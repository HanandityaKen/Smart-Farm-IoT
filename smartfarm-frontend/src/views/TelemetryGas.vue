<template>
  <div class="space-y-6">
    <!-- Header Halaman & Pencarian -->
    <div class="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-4">
      <div>
        <h1 class="text-xl font-bold text-slate-800 dark:text-slate-100 flex items-center gap-2">
          <svg class="w-5 h-5 text-amber-500" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24">
            <path d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z"/>
          </svg>
          Riwayat Gas Amonia & Metana
        </h1>
        <p class="text-xs text-slate-500 dark:text-slate-400 mt-0.5">Monitoring konsentrasi gas berbahaya, batas amonia/metana, dan log pembersihan kandang.</p>
      </div>

      <!-- Filter Pencarian -->
      <div class="flex items-center gap-2">
        <input 
          v-model="searchQuery" 
          type="text" 
          placeholder="Cari data..." 
          class="px-3 py-1.5 text-xs bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 rounded-lg focus:outline-none focus:border-amber-500 text-slate-800 dark:text-slate-200"
        />
      </div>
    </div>

    <!-- Ringkasan Statistik Gas -->
    <div class="grid grid-cols-1 sm:grid-cols-3 gap-4">
      <!-- Konsentrasi Gas Terakhir -->
      <div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
        <p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Konsentrasi Gas Terakhir</p>
        <div class="flex items-baseline gap-2 mt-2">
          <span class="text-2xl font-bold text-slate-800 dark:text-slate-100">
            {{ latestLog.gas_ppm !== undefined ? latestLog.gas_ppm : (latestLog.gas || '--') }} PPM
          </span>
          <span 
            :class="[
              'text-[10px] px-2 py-0.5 rounded-full font-semibold',
              (latestLog.gas_ppm || latestLog.gas || 0) > (config.batas_gas || 2500) 
                ? 'bg-rose-500/10 text-rose-600 dark:text-rose-400 border border-rose-500/20' 
                : 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border border-emerald-500/20'
            ]"
          >
            {{ (latestLog.gas_ppm || latestLog.gas || 0) > (config.batas_gas || 2500) ? 'Gas Melebihi Ambang' : 'Kualitas Udara Baik' }}
          </span>
        </div>
      </div>

      <!-- Batas Ambang Gas -->
      <div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
        <p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Batas Ambang Gas (PPM)</p>
        <div class="flex items-baseline gap-2 mt-2">
          <span class="text-2xl font-bold text-slate-800 dark:text-slate-100">
            {{ config.batas_gas || 2500 }} PPM
          </span>
          <span class="text-xs text-slate-400">Batas Bahaya</span>
        </div>
      </div>

      <!-- Jadwal Pembersihan Berikutnya -->
      <div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
        <p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Status Pembersihan Kandang</p>
        <div class="flex items-center gap-2 mt-2">
          <span 
            :class="[
              'text-xs font-bold px-2.5 py-1 rounded-lg',
              (latestLog.gas_ppm || latestLog.gas || 0) > (config.batas_gas || 2500)
                ? 'bg-amber-500/10 text-amber-600 dark:text-amber-400 border border-amber-500/20'
                : 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border border-emerald-500/20'
            ]"
          >
            {{ (latestLog.gas_ppm || latestLog.gas || 0) > (config.batas_gas || 2500) ? 'Perlu Pembersihan' : 'Kandang Bersih' }}
          </span>
        </div>
      </div>
    </div>

    <!-- Tabel Data Log Gas -->
    <div class="bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl overflow-hidden shadow-sm">
      <div class="overflow-x-auto">
        <table class="w-full text-left text-xs border-collapse">
          <thead>
            <tr class="border-b border-slate-200 dark:border-slate-800 bg-slate-50/50 dark:bg-slate-800/40 text-slate-500 dark:text-slate-400 uppercase tracking-wider font-semibold">
              <th class="py-3 px-4">Waktu & Tanggal</th>
              <th class="py-3 px-4">Konsentrasi Gas (PPM)</th>
              <th class="py-3 px-4">Batas Ambang (PPM)</th>
              <th class="py-3 px-4">Jadwal Pembersihan</th>
              <th class="py-3 px-4">Keterangan Status</th>
            </tr>
          </thead>
          <tbody class="divide-y divide-slate-200 dark:divide-slate-800/60 text-slate-700 dark:text-slate-300">
            <tr 
              v-for="(log, index) in filteredGasLogs" 
              :key="log.id || index"
              class="hover:bg-slate-50/80 dark:hover:bg-slate-800/30 transition-colors"
            >
              <!-- Waktu & Tanggal -->
              <td class="py-3 px-4 whitespace-nowrap font-medium text-slate-800 dark:text-slate-200">
                {{ log.created_at ? formatDate(log.created_at) : (log.waktu || '-') }}
              </td>

              <!-- Konsentrasi Gas -->
              <td class="py-3 px-4 whitespace-nowrap font-semibold">
                <span :class="(log.gas_ppm || log.gas) > (config.batas_gas || 2500) ? 'text-rose-500 font-bold' : ''">
                  {{ log.gas_ppm !== undefined ? log.gas_ppm : (log.gas !== undefined ? log.gas : '-') }} PPM
                </span>
              </td>

              <!-- Batas Ambang -->
              <td class="py-3 px-4 whitespace-nowrap text-slate-500">
                {{ config.batas_gas || 2500 }} PPM
              </td>

              <!-- Jadwal Pembersihan -->
              <td class="py-3 px-4 whitespace-nowrap text-slate-600 dark:text-slate-400">
                {{ log.jadwal_pembersihan || ( (log.gas_ppm || log.gas) > (config.batas_gas || 2500) ? 'Hari ini (Segera)' : '-' ) }}
              </td>

              <!-- Keterangan Status -->
              <td class="py-3 px-4 max-w-xs truncate">
                <span 
                  :class="[
                    'inline-flex items-center gap-1.5 px-2 py-0.5 rounded text-[10px] font-semibold',
                    (log.gas_ppm || log.gas) > (config.batas_gas || 2500)
                      ? 'bg-rose-500/10 text-rose-600 dark:text-rose-400 border border-rose-500/20'
                      : 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border border-emerald-500/20'
                  ]"
                >
                  <span 
                    :class="[
                      'w-1.5 h-1.5 rounded-full',
                      (log.gas_ppm || log.gas) > (config.batas_gas || 2500) ? 'bg-rose-500 animate-ping' : 'bg-emerald-500'
                    ]"
                  ></span>
                  {{ log.keterangan || ((log.gas_ppm || log.gas) > (config.batas_gas || 2500) ? 'Gas Melebihi Ambang' : 'Gas Kembali Normal / Kandang Bersih') }}
                </span>
              </td>
            </tr>

            <!-- State Kosong -->
            <tr v-if="filteredGasLogs.length === 0">
              <td colspan="5" class="py-8 text-center text-slate-400">
                Belum ada data log gas yang terekam.
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
    gasLogs: {
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
    filteredGasLogs() {
      if (!Array.isArray(this.gasLogs)) return [];

      return this.gasLogs.filter(log => {
        // Abaikan jika data gas tidak ada
        if (log.gas_ppm === undefined || log.gas_ppm === null) return false;
        
        // Jika kotak pencarian kosong, langsung tampilkan semua data
        const q = this.searchQuery.trim().toLowerCase();
        if (!q) return true; 

        const time = (this.formatDate ? this.formatDate(log.created_at) : (log.created_at || '')).toLowerCase();
        const gas = log.gas_ppm.toString().toLowerCase();
        const jadwal_pembersihan = log.jadwal_pembersihan.toString().toLowerCase();
        
        return time.includes(q) || gas.includes(q) || jadwal_pembersihan.includes(q);
      });
    }
  }
}
</script>