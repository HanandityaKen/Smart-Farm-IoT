<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\KontrolController;
use App\Http\Controllers\Api\SensorController;
use App\Http\Controllers\Api\LogSuhuController;
use App\Http\Controllers\Api\LogGasController;
use App\Http\Controllers\Api\LogPakanMinumController;

Route::get('/user', function (Request $request) {
    return $request->user();
})->middleware('auth:sanctum');

// ==========================================
// API UNTUK FRONTEND DASHBOARD (VUE 3)
// ==========================================

// Dashboard Real-time Data
Route::get('/sensor', [SensorController::class, 'getSensorLogs']);

// Konfigurasi Sistem (Threshold & Jadwal)
Route::get('/konfigurasi', [SensorController::class, 'getConfig']);
Route::put('/konfigurasi', [SensorController::class, 'updateConfig']);

// Kontrol Sakelar (Menyimpan status & Memicu MQTT Publish ke ESP32)
Route::get('/kontrol', [KontrolController::class, 'index']);
Route::patch('/kontrol', [KontrolController::class, 'update']);

// Riwayat Logs untuk Tabel & Grafik
Route::get('/telemetri/suhu', [LogSuhuController::class, 'index']);
Route::get('/telemetri/gas', [LogGasController::class, 'index']);
Route::get('/telemetri/pakan-minum', [LogPakanMinumController::class, 'index']);

// ==========================================
// FALLBACK REST API (OPSIONAL / UNTUK POSTMAN)
// ==========================================
Route::patch('/sensor', [SensorController::class, 'updateSensorLog']);
Route::post('/telemetri/suhu', [LogSuhuController::class, 'store']);
Route::post('/telemetri/gas', [LogGasController::class, 'store']);
Route::post('/telemetri/pakan-minum', [LogPakanMinumController::class, 'store']);