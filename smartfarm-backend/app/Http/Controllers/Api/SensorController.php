<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;
use App\Models\SensorLog;
use App\Models\KonfigurasiSistem;
use PhpMqtt\Client\MqttClient;
use PhpMqtt\Client\ConnectionSettings;

class SensorController extends Controller
{
    public function getSensorLogs() {
        $logs = SensorLog::latest()->take(10)->get()->reverse()->values();
        return response()->json(['status' => 'success', 'data' => $logs]);
    }

    public function updateSensorLog(Request $request)
    {
        $validated = $request->validate([
            'suhu'         => 'required|numeric',
            'gas_ppm'      => 'required|integer',
            'sisa_pakan'   => 'required|numeric',
            'status_kipas' => 'required|string',
        ]);

        $log = SensorLog::first();

        if ($log) {
            $log->fill($validated);
            $log->updated_at = now(); 
            $log->save();
            $log = $log->fresh(); 
        } else {
            $log = SensorLog::create($validated);
        }

        return response()->json([
            'status'  => 'success',
            'message' => 'Data sensor berhasil diperbarui',
            'data'    => $log
        ], 200);
    }

    public function getConfig()
    {
        $config = KonfigurasiSistem::first();
        return response()->json(['status' => 'success', 'data' => $config]);
    }

    public function updateConfig(Request $request)
    {
        $validated = $request->validate([
            'batas_gas'       => 'nullable|numeric',
            'batas_suhu'      => 'nullable|numeric',
            'jam_pakan_1'     => 'nullable',
            'jam_pakan_2'     => 'nullable',
            'jam_pakan_3'     => 'nullable',
            'jam_lampu_mati'  => 'nullable',
            'jam_lampu_nyala' => 'nullable',
        ]);

        // Simpan/Update ke Database Supabase
        $config = KonfigurasiSistem::first();
        if ($config) {
            $config->update($validated);
            $config->refresh();
        } else {
            $config = KonfigurasiSistem::create($validated);
        }

        // Publish Konfigurasi Terbaru ke HiveMQ Cloud (MQTT) agar ESP32 Tahu
        try {
            $server = config('mqtt-client.connections.default.host');
            $port   = (int) config('mqtt-client.connections.default.port', 8883);
            $pubId  = 'sf_cfg_' . rand(100, 999);

            $mqtt = new MqttClient($server, $port, $pubId);

            $settings = (new ConnectionSettings)
                ->setUseTls(true)
                ->setUsername(config('mqtt-client.connections.default.connection_settings.auth.username'))
                ->setPassword(config('mqtt-client.connections.default.connection_settings.auth.password'));

            $payload = [
                'batas_gas'       => (int) $config->batas_gas,
                'batas_suhu'      => (float) $config->batas_suhu,
                'jam_pakan_1'     => (string) $config->jam_pakan_1,
                'jam_pakan_2'     => (string) $config->jam_pakan_2,
                'jam_pakan_3'     => (string) $config->jam_pakan_3,
                'jam_lampu_mati'  => (string) $config->jam_lampu_mati,
                'jam_lampu_nyala' => (string) $config->jam_lampu_nyala,
            ];

            $mqtt->connect($settings, true);
            $mqtt->publish('smartfarm/konfigurasi', json_encode($payload), 0);
            $mqtt->disconnect();
        } catch (\Exception $e) {
            \Log::error('Gagal publish konfigurasi ke MQTT: ' . $e->getMessage());
        }

        return response()->json([
            'status'  => 'success',
            'message' => 'Konfigurasi berhasil diperbarui & dikirim ke ESP32',
            'data'    => $config
        ]);
    }
}