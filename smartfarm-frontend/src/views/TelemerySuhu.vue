<template>
  <div class="space-y-6">
    <!-- Header Halaman & Filter -->
    <div class="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-4">
      <div>
        <h1 class="text-xl font-bold text-slate-800 dark:text-slate-100 flex items-center gap-2">
          <svg class="w-5 h-5 text-orange-500" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24">
            <path d="M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z"/>
          </svg>
          Riwayat Suhu Kandang
        </h1>
        <p class="text-xs text-slate-500 dark:text-slate-400 mt-0.5">Monitoring perubahan suhu, batas ambang, dan status aktivasi kipas pendingin.</p>
      </div>

      <!-- Pencarian / Filter Sederhana -->
      <div class="flex items-center gap-2">
        <input 
          v-model="searchQuery" 
          type="text" 
          placeholder="Cari data..." 
          class="px-3 py-1.5 text-xs bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 rounded-lg focus:outline-none focus:border-emerald-500 text-slate-800 dark:text-slate-200"
        />
      </div>
    </div>

    <!-- Ringkasan Statistik Suhu -->
    <div class="grid grid-cols-1 sm:grid-cols-3 gap-4">
      <!-- Suhu Saat Ini -->
      <div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
        <p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Suhu Terakhir</p>
        <div class="flex items-baseline gap-2 mt-2">
          <span class="text-2xl font-bold text-slate-800 dark:text-slate-100">
            {{ latestLog.suhu !== undefined ? latestLog.suhu : '--' }} °C
          </span>
          <span 
            :class="[
              'text-[10px] px-2 py-0.5 rounded-full font-semibold',
              (latestLog.suhu || 0) > (config.batas_suhu || 30) 
                ? 'bg-rose-500/10 text-rose-600 dark:text-rose-400 border border-rose-500/20' 
                : 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border border-emerald-500/20'
            ]"
          >
            {{ (latestLog.suhu || 0) > (config.batas_suhu || 30) ? 'Suhu Tinggi' : 'Normal' }}
          </span>
        </div>
      </div>

      <!-- Batas Ambang Suhu -->
      <div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
        <p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Batas Ambang Kipas</p>
        <div class="flex items-baseline gap-2 mt-2">
          <span class="text-2xl font-bold text-slate-800 dark:text-slate-100">
            {{ config.batas_suhu || 30 }} °C
          </span>
          <span class="text-xs text-slate-400">Target Otomatis</span>
        </div>
      </div>

      <!-- Status Kipas Pembawa -->
      <div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
        <p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Status Kipas Ekstraktor</p>
        <div class="flex items-center gap-2 mt-2">
          <span 
            :class="[
              'text-lg font-bold px-2.5 py-0.5 rounded-lg',
              (latestLog.status_kipas == 1 || latestLog.status_kipas === 'ON' || latestLog.status_kipas === true)
                ? 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border border-emerald-500/20' 
                : 'bg-slate-100 dark:bg-slate-800 text-slate-500 dark:text-slate-400'
            ]"
          >
            {{ (latestLog.status_kipas == 1 || latestLog.status_kipas === 'ON' || latestLog.status_kipas === true) ? 'ON' : 'OFF' }}
          </span>
          <span class="text-xs text-slate-500 dark:text-slate-400">
            {{ latestLog.mode_kontrol || 'Otomatis' }}
          </span>
        </div>
      </div>
    </div>

    <!-- Tabel Data Log Suhu -->
    <div class="bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl overflow-hidden shadow-sm">
      <div class="overflow-x-auto">
        <table class="w-full text-left text-xs border-collapse">
          <thead>
            <tr class="border-b border-slate-200 dark:border-slate-800 bg-slate-50/50 dark:bg-slate-800/40 text-slate-500 dark:text-slate-400 uppercase tracking-wider font-semibold">
              <th class="py-3 px-4">Waktu & Tanggal</th>
              <th class="py-3 px-4">Suhu (°C)</th>
              <th class="py-3 px-4">Batas Ambang (°C)</th>
              <th class="py-3 px-4">Status Kipas</th>
              <th class="py-3 px-4">Mode Kontrol</th>
              <th class="py-3 px-4">Keterangan</th>
            </tr>
          </thead>
          <tbody class="divide-y divide-slate-200 dark:divide-slate-800/60 text-slate-700 dark:text-slate-300">
            <tr 
              v-for="(log, index) in filteredSuhuLogs" 
              :key="log.id || index"
              class="hover:bg-slate-50/80 dark:hover:bg-slate-800/30 transition-colors"
            >
              <!-- Waktu & Tanggal -->
              <td class="py-3 px-4 whitespace-nowrap font-medium text-slate-800 dark:text-slate-200">
                {{ log.created_at ? formatDate(log.created_at) : '-' }}
              </td>

              <!-- Suhu -->
              <td class="py-3 px-4 whitespace-nowrap font-semibold">
                <span :class="log.suhu > (config.batas_suhu || 30) ? 'text-rose-500 font-bold' : ''">
                  {{ log.suhu !== undefined ? log.suhu : '-' }} °C
                </span>
              </td>

              <!-- Batas Ambang -->
              <td class="py-3 px-4 whitespace-nowrap text-slate-500">
                {{ log.batas_ambang || 30 }} °C
              </td>

              <!-- Status Kipas -->
              <td class="py-3 px-4 whitespace-nowrap">
                <span 
                  :class="[
                    'inline-flex items-center gap-1 px-2 py-0.5 rounded text-[10px] font-bold',
                    log.status_kipas === 'ON' 
                      ? 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border border-emerald-500/20' 
                      : 'bg-slate-100 dark:bg-slate-800 text-slate-500'
                  ]"
                >
                  <span v-if="log.status_kipas === 'ON'" class="w-1.5 h-1.5 rounded-full bg-emerald-500 animate-pulse"></span>
                  {{ log.status_kipas || 'OFF' }}
                </span>
              </td>

              <!-- Mode Kontrol -->
              <td class="py-3 px-4 whitespace-nowrap">
                <span class="px-2 py-0.5 rounded text-[10px] font-medium bg-slate-100 dark:bg-slate-800 text-slate-600 dark:text-slate-400 border border-slate-200 dark:border-slate-700">
                  {{ log.mode_kontrol || 'Otomatis' }}
                </span>
              </td>

              <!-- Keterangan -->
              <td class="py-3 px-4 text-slate-500 dark:text-slate-400 max-w-xs truncate">
                {{ log.keterangan || (log.suhu > (config.batas_suhu || 30) ? 'Suhu Tinggi' : 'Suhu Kembali Normal') }}
              </td>
            </tr>

            <!-- State Kosong jika Tidak Ada Data -->
            <tr v-if="filteredSuhuLogs.length === 0">
              <td colspan="6" class="py-8 text-center text-slate-400">
                Belum ada data log suhu yang terekam.
              </td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    suhuLogs: {
      type: Array,
      default: () => []
    },
    latestLog: {
      type: Object,
      default: () => ({})
    },
    config: {
      type: Object,
      default: () => ({})
    },
    formatDate: {
      type: Function,
      default: (val) => val
    }
  },
  data() {
    return {
      searchQuery: ''
    }
  },
  computed: {
    filteredSuhuLogs() {
      // Pastikan suhuLogs berbentuk array sebelum difilter
      if (!Array.isArray(this.suhuLogs)) return [];

      return this.suhuLogs.filter(log => {
        // Abaikan jika data suhu tidak ada
        if (log.suhu === undefined || log.suhu === null) return false;
        
        // Jika kotak pencarian kosong, langsung tampilkan semua data
        const q = this.searchQuery.trim().toLowerCase();
        if (!q) return true; 

        const time = (this.formatDate ? this.formatDate(log.created_at) : (log.created_at || '')).toLowerCase();
        const suhu = log.suhu.toString().toLowerCase();
        const mode = (log.mode_kontrol || '').toLowerCase();
        
        return time.includes(q) || suhu.includes(q) || mode.includes(q);
      });
    }
  }
}
</script>