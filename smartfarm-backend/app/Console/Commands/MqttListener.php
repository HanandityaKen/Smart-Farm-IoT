<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;
use PhpMqtt\Client\MqttClient;
use PhpMqtt\Client\ConnectionSettings;
use App\Models\SensorLog;
use App\Models\LogSuhu;
use App\Models\LogGas;
use App\Models\LogPakanMinum;
use Illuminate\Support\Facades\Log;

class MqttListener extends Command
{
    protected $signature = 'app:mqtt-listener';
    protected $description = 'Mendengarkan data sensor & log ESP32 via HiveMQ Cloud';

    public function handle()
    {
        $this->info('Menghubungkan ke HiveMQ Cloud...');

        try {
            $server   = config('mqtt-client.connections.default.host');
            $port     = (int) config('mqtt-client.connections.default.port', 8883);
            $listenId = 'sflisten' . rand(100, 999);

            $mqtt = new MqttClient($server, $port, $listenId);

            $settings = (new ConnectionSettings)
                ->setUseTls(true)
                ->setKeepAliveInterval(10)
                ->setUsername(config('mqtt-client.connections.default.connection_settings.auth.username'))
                ->setPassword(config('mqtt-client.connections.default.connection_settings.auth.password'));

            $mqtt->connect($settings, true);

            // 1. Subscribe Telemetri Realtime Sensor
            $mqtt->subscribe('smartfarm/telemetri', function (string $topic, string $message) {
                // $this->info("Telemetri Masuk [{$topic}]: {$message}");
                $data = json_decode($message, true);

                if ($data) {
                    $log = SensorLog::first();
                    $payload = [
                        'suhu'         => $data['suhu'] ?? 0,
                        'gas_ppm'      => $data['gas_ppm'] ?? 0,
                        'sisa_pakan'   => $data['sisa_pakan'] ?? 0,
                        'status_kipas' => $data['status_kipas'] ?? 'OFF',
                    ];

                    if ($log) {
                        $log->update($payload);
                    } else {
                        SensorLog::create($payload);
                    }
                    // $this->info('-> SensorLog diperbarui.');
                }
            });

            // 2. Subscribe Log Suhu & Kipas
            $mqtt->subscribe('smartfarm/log/suhu', function (string $topic, string $message) {
                $this->info("Log Suhu Masuk [{$topic}]: {$message}");
                $data = json_decode($message, true);
                if ($data) {
                    LogSuhu::create([
                        'suhu'         => $data['suhu'] ?? 0,
                        'batas_ambang' => $data['batas_ambang'] ?? 32.0,
                        'status_kipas' => $data['status_kipas'] ?? 'OFF',
                        'mode_kontrol' => $data['mode_kontrol'] ?? 'Otomatis',
                        'keterangan'   => $data['keterangan'] ?? 'Log Suhu',
                    ]);
                    $this->info('-> LogSuhu tersimpan ke Supabase.');
                }
            });

            // 3. Subscribe Log Gas
            $mqtt->subscribe('smartfarm/log/gas', function (string $topic, string $message) {
                $this->info("Log Gas Masuk [{$topic}]: {$message}");
                $data = json_decode($message, true);
                if ($data) {
                    LogGas::create([
                        'gas_ppm'            => $data['gas_ppm'] ?? 0,
                        'batas_gas'          => $data['batas_gas'] ?? 2500,
                        'jadwal_pembersihan' => $data['jadwal_pembersihan'] ?? '-',
                        'keterangan'         => $data['keterangan'] ?? 'Log Gas',
                    ]);
                    $this->info('-> LogGas tersimpan ke Supabase.');
                }
            });

            // 4. Subscribe Log Pakan & Minum
            $mqtt->subscribe('smartfarm/log/pakan-minum', function (string $topic, string $message) {
                $this->info("Log Pakan/Minum Masuk [{$topic}]: {$message}");
                $data = json_decode($message, true);
                if ($data) {
                    LogPakanMinum::create([
                        'jenis_aksi'   => $data['jenis_aksi'] ?? 'Pakan',
                        'sisa_pakan'   => $data['sisa_pakan'] ?? 0,
                        'mode_kontrol' => $data['mode_kontrol'] ?? 'Manual',
                        'keterangan'   => $data['keterangan'] ?? 'Aksi Pakan/Minum',
                    ]);
                    $this->info('-> LogPakanMinum tersimpan ke Supabase.');
                }
            });

            $mqtt->loop(true);

        } catch (\Exception $e) {
            $this->error("Error Koneksi MQTT: " . $e->getMessage());
            Log::error("MQTT Listener Error: " . $e->getMessage());
        }

        return Command::SUCCESS;
    }
}