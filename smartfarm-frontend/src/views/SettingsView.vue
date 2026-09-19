<template>
  <div class="w-full space-y-6 relative">
    
    <!-- TOAST NOTIFICATION (Kustom Tailwind) -->
    <Transition name="slide-fade">
      <div 
        v-if="toast.show" 
        :class="[
          'fixed top-5 right-5 z-50 flex items-center gap-3 px-4 py-3 rounded-xl shadow-lg border text-xs font-medium transition-all duration-300',
          toast.type === 'success' 
            ? 'bg-emerald-500/10 border-emerald-500/30 text-emerald-600 dark:text-emerald-400 backdrop-blur-md bg-white/80 dark:bg-slate-900/80' 
            : 'bg-rose-500/10 border-rose-500/30 text-rose-600 dark:text-rose-400 backdrop-blur-md bg-white/80 dark:bg-slate-900/80'
        ]"
      >
        <span>{{ toast.type === 'success' ? '✅' : '⚠️' }}</span>
        <span>{{ toast.message }}</span>
      </div>
    </Transition>

    <div class="p-6 rounded-2xl bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 shadow-sm">
      
      <!-- HEADER SETTINGS -->
      <div class="border-b border-slate-200 dark:border-slate-800 pb-4 mb-6">
        <h2 class="text-base font-semibold text-slate-800 dark:text-slate-100">Konfigurasi Batas Kritis (Threshold) & Jadwal</h2>
        <p class="text-xs text-slate-500 dark:text-slate-400 mt-1">
          Atur parameter batas suhu, kadar gas amonia, jadwal pakan otomatis, dan kontrol lampu pada hardware ESP32.
        </p>
      </div>

      <div class="space-y-6">
        
        <!-- SEKSI 1: THRESHOLD SENSOR (GRID 2 KOLOM) -->
        <div>
          <h3 class="text-xs font-bold text-emerald-600 dark:text-emerald-400 uppercase tracking-wider mb-3">1. Threshold Sensor Kipas</h3>
          <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
            <!-- Form Batas Suhu -->
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">
                Batas Suhu Kipas Nyala (°C)
              </label>
              <div class="relative w-full">
                <input 
                  type="number" 
                  step="0.5" 
                  v-model="config.batas_suhu"
                  class="w-full bg-slate-50 dark:bg-slate-950 border border-slate-300 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-800 dark:text-slate-200 focus:outline-none focus:border-emerald-500 transition"
                  placeholder="Contoh: 31.5"
                />
                <span class="absolute right-3 top-2.5 text-xs text-slate-400 font-medium">°C</span>
              </div>
            </div>

            <!-- Form Batas Gas -->
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">
                Batas Toleransi Gas Amonia (PPM)
              </label>
              <div class="relative w-full">
                <input 
                  type="number" 
                  step="50"
                  v-model="config.batas_gas"
                  class="w-full bg-slate-50 dark:bg-slate-950 border border-slate-300 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-800 dark:text-slate-200 focus:outline-none focus:border-emerald-500 transition"
                  placeholder="Contoh: 2400"
                />
                <span class="absolute right-3 top-2.5 text-xs text-slate-400 font-medium">PPM</span>
              </div>
            </div>
          </div>
        </div>

        <hr class="border-slate-200 dark:border-slate-800/60" />

        <!-- SEKSI 2: JADWAL PAKAN OTOMATIS (GRID 3 KOLOM) -->
        <div>
          <h3 class="text-xs font-bold text-emerald-600 dark:text-emerald-400 uppercase tracking-wider mb-3">2. Jadwal Pemberian Pakan</h3>
          <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Jam Pakan 1 (Pagi)</label>
              <input 
                type="time" 
                v-model="config.jam_pakan_1"
                class="w-full bg-slate-50 dark:bg-slate-950 border border-slate-300 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-800 dark:text-slate-200 focus:outline-none focus:border-emerald-500 transition"
              />
            </div>
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Jam Pakan 2 (Siang)</label>
              <input 
                type="time" 
                v-model="config.jam_pakan_2"
                class="w-full bg-slate-50 dark:bg-slate-950 border border-slate-300 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-800 dark:text-slate-200 focus:outline-none focus:border-emerald-500 transition"
              />
            </div>
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Jam Pakan 3 (Sore)</label>
              <input 
                type="time" 
                v-model="config.jam_pakan_3"
                class="w-full bg-slate-50 dark:bg-slate-950 border border-slate-300 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-800 dark:text-slate-200 focus:outline-none focus:border-emerald-500 transition"
              />
            </div>
          </div>
        </div>

        <hr class="border-slate-200 dark:border-slate-800/60" />

        <!-- SEKSI 3: JADWAL LAMPU NYALA & MATI (GRID 2 KOLOM) -->
        <div>
          <h3 class="text-xs font-bold text-emerald-600 dark:text-emerald-400 uppercase tracking-wider mb-3">3. Kontrol Lampu Otomatis</h3>
          <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Jam Lampu Nyala</label>
              <input 
                type="time" 
                v-model="config.jam_lampu_nyala"
                class="w-full bg-slate-50 dark:bg-slate-950 border border-slate-300 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-800 dark:text-slate-200 focus:outline-none focus:border-emerald-500 transition"
              />
            </div>
            <!-- Tampilan Rentang Waktu (Jam Nyala - Jam Mati Otomatis) -->
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Rentang Waktu Aktif</label>
              <div class="w-full bg-slate-100 dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-700 dark:text-slate-300 flex items-center justify-between select-none">
                <span class="font-mono font-semibold">
                  {{ config.jam_lampu_nyala ? config.jam_lampu_nyala.slice(0, 5) : '00:00' }} - {{ config.jam_lampu_mati ? config.jam_lampu_mati.slice(0, 5) : '00:00' }}
                </span>
                <span class="text-[10px] bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 px-2 py-0.5 rounded font-medium">
                  Otomatis
                </span>
              </div>
              <p class="text-[11px] text-slate-400 mt-1">Lampu akan mati otomatis pada pukul {{ config.jam_lampu_mati ? config.jam_lampu_mati.slice(0, 5) : '00:00' }}.</p>
            </div>
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Jam Lampu Mati</label>
              <input 
                type="time" 
                v-model="config.jam_lampu_mati"
                class="w-full bg-slate-50 dark:bg-slate-950 border border-slate-300 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-800 dark:text-slate-200 focus:outline-none focus:border-emerald-500 transition"
              />
            </div>
            <!-- Tampilan Rentang Waktu (Jam Nyala - Jam Mati Otomatis) -->
            <div>
              <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Rentang Waktu Nonaktif</label>
              <div class="w-full bg-slate-100 dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-lg px-3 py-2 text-sm text-slate-700 dark:text-slate-300 flex items-center justify-between select-none">
                <span class="font-mono font-semibold">
                  {{ config.jam_lampu_mati ? config.jam_lampu_mati.slice(0, 5) : '00:00' }} - {{ config.jam_lampu_nyala ? config.jam_lampu_nyala.slice(0, 5) : '00:00' }}
                </span>
                <span class="text-[10px] bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 px-2 py-0.5 rounded font-medium">
                  Otomatis
                </span>
              </div>
              <p class="text-[11px] text-slate-400 mt-1">Lampu akan nyala otomatis pada pukul {{ config.jam_lampu_nyala ? config.jam_lampu_nyala.slice(0, 5) : '00:00' }}.</p>
            </div>
          </div>
        </div>

      </div>

      <!-- TOMBOL SIMPAN -->
      <div class="pt-6 mt-6 border-t border-slate-200 dark:border-slate-800">
        <button 
          @click="saveConfig" 
          :disabled="loading"
          class="px-5 py-2.5 bg-emerald-600 hover:bg-emerald-500 text-white font-medium text-xs rounded-lg shadow-sm transition disabled:opacity-50 flex items-center gap-2"
        >
          <span v-if="loading" class="w-3.5 h-3.5 border-2 border-white/30 border-t-white rounded-full animate-spin"></span>
          {{ loading ? 'Menyimpan...' : 'Simpan Perubahan' }}
        </button>
      </div>

    </div>
  </div>
</template>

<script>
import axios from 'axios'

export default {
  props: {
    config: {
      type: Object,
      default: () => ({})
    },
  },
  data() {
    return {
      loading: false,
      toast: {
        show: false,
        message: '',
        type: 'success'
      }
    }
  },
  methods: {
    showToast(msg, type = 'success') {
      this.toast.message = msg
      this.toast.type = type
      this.toast.show = true
      setTimeout(() => {
        this.toast.show = false
      }, 3000)
    },
    async saveConfig() {
      this.loading = true
      try {
        await axios.put('/konfigurasi', this.config)
        this.showToast('Konfigurasi berhasil disimpan!', 'success')
      } catch (err) {
        this.showToast('Gagal menyimpan konfigurasi ke server', 'error')
      } finally {
        this.loading = false
      }
    }
  }
}
</script>

<style scoped>
.slide-fade-enter-active,
.slide-fade-leave-active {
  transition: all 0.3s ease-out;
}
.slide-fade-enter-from,
.slide-fade-leave-to {
  transform: translateY(-20px);
  opacity: 0;
}
</style>