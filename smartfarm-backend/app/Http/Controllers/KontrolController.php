<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\Models\KontrolPerangkat;
use PhpMqtt\Client\MqttClient;
use PhpMqtt\Client\ConnectionSettings;
use Illuminate\Support\Facades\Log;

class KontrolController extends Controller
{
    // GET /api/kontrol
    public function index()
    {
        $kontrol = KontrolPerangkat::find(1);
        return response()->json($kontrol);
    }

    // PATCH /api/kontrol
    public function update(Request $request)
    {
        $kontrol = KontrolPerangkat::find(1);

        if (!$kontrol) {
            return response()->json(['message' => 'Data tidak ditemukan'], 404);
        }

        $dataToUpdate = $request->only([
            'lampu_dalam',
            'lampu_luar',
            'kipas',
            'trigger_pakan',
            'trigger_pompa'
        ]);

        $kontrol->update($dataToUpdate);

        $payload = [
            'lampu_dalam'   => (bool)$kontrol->lampu_dalam,
            'lampu_luar'    => (bool)$kontrol->lampu_luar,
            'kipas'         => (bool)$kontrol->kipas,
            'trigger_pakan' => (bool)$kontrol->trigger_pakan,
            'trigger_pompa' => (bool)$kontrol->trigger_pompa,
        ];

        // MENGIRIM PERINTAH KE ESP32 VIA MQTT
        try {
            $server = config('mqtt-client.connections.default.host');
            $port   = (int) config('mqtt-client.connections.default.port', 8883);
            $pubId  = 'sfpub' . rand(1000, 9999);

            $mqtt = new MqttClient($server, $port, $pubId);

            // Optimasi batas timeout koneksi SSL agar tidak menggantung (blocking)
            $settings = (new ConnectionSettings)
                ->setUseTls(true)
                ->setConnectTimeout(3) // Maksimal 3 detik untuk handshake TLS
                ->setSocketTimeout(3)
                ->setUsername(config('mqtt-client.connections.default.connection_settings.auth.username'))
                ->setPassword(config('mqtt-client.connections.default.connection_settings.auth.password'));

            $mqtt->connect($settings, true);
            // QoS 0 agar pesan dikirim secara instan (Fire and Forget)
            $mqtt->publish('smartfarm/kontrol', json_encode($payload), 0);
            $mqtt->disconnect();
        } catch (\Exception $e) {
            Log::error('Gagal Publish MQTT Kontrol: ' . $e->getMessage());
        }

        // Otomatis reset status trigger agar DB & UI sync
        if ($request->has('trigger_pakan') || $request->has('trigger_pompa')) {
            $kontrol->update([
                'trigger_pakan' => false,
                'trigger_pompa' => false,
            ]);
            $kontrol->refresh();
        }

        return response()->json([
            'message' => 'Status kontrol berhasil diperbarui & perintah terkirim via MQTT',
            'data'    => $kontrol
        ]);
    }
}