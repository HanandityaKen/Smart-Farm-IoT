<template>
  <div class="space-y-6">
    <!-- Header Halaman & Filter -->
    <div class="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-4">
      <div>
        <h1 class="text-xl font-bold text-slate-800 dark:text-slate-100 flex items-center gap-2">
          <svg class="w-5 h-5 text-cyan-500" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24">
            <path d="m16 2-2.3 2.3a3 3 0 0 0 0 4.2l1.8 1.8a3 3 0 0 0 4.2 0L22 8"/>
            <path d="M15 15 3.3 3.3a4.2 4.2 0 0 0 0 6l7.3 7.3c.7.7 2 .7 2.8 0L15 15Zm0 0 7 7"/>
            <path d="m2.1 21.9 8.8-8.8"/>
          </svg>
          Riwayat Pakan & Minum
        </h1>
        <p class="text-xs text-slate-500 dark:text-slate-400 mt-0.5">Monitoring jadwal pemberian pakan/minum otomatis dan sisa stok di wadah.</p>
      </div>

      <!-- Filter Sederhana -->
      <div class="flex items-center gap-2">
        <input 
          v-model="searchQuery" 
          type="text" 
          placeholder="Cari jenis / mode..." 
          class="px-3 py-1.5 text-xs bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 rounded-lg focus:outline-none focus:border-cyan-500 text-slate-800 dark:text-slate-200"
        />
      </div>
    </div>

    <!-- Ringkasan Statistik -->
    <div class="grid grid-cols-1 sm:grid-cols-3 gap-4">
      <!-- Sisa Pakan Terakhir -->
      <div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
        <p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Sisa Pakan di Wadah</p>
        <div class="flex items-baseline gap-2 mt-2">
          <span class="text-2xl font-bold text-slate-800 dark:text-slate-100">
            {{ latestPakanLog.sisa_pakan !== undefined ? latestPakanLog.sisa_pakan : '--' }}%
          </span>
          <span 
            :class="[
              'text-[10px] px-2 py-0.5 rounded-full font-semibold',
              (latestPakanLog.sisa_pakan || 0) < 20 
                ? 'bg-rose-500/10 text-rose-600 dark:text-rose-400 border border-rose-500/20' 
                : 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border border-emerald-500/20'
            ]"
          >
            {{ (latestPakanLog.sisa_pakan || 0) < 20 ? 'Perlu Isi Ulang' : 'Cukup' }}
          </span>
        </div>
      </div>

      <!-- Aktivitas Pakan Terakhir -->
			<div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
				<p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Aktivitas Pakan Terakhir</p>
				<div class="flex items-baseline justify-between gap-2 mt-2">
					<span class="text-lg font-bold text-amber-600 dark:text-amber-400">
						Pakan
					</span>
					<span class="text-xs text-slate-500 dark:text-slate-400">
						{{ latestPakanLog.created_at ? formatDate(latestPakanLog.created_at) : (latestPakanLog.waktu || '-') }}
					</span>
				</div>
			</div>

			<!-- Aktivitas Minum Terakhir -->
			<div class="p-4 bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl">
				<p class="text-[11px] font-medium text-slate-500 dark:text-slate-400 uppercase tracking-wider">Aktivitas Minum Terakhir</p>
				<div class="flex items-baseline justify-between gap-2 mt-2">
					<span class="text-lg font-bold text-cyan-600 dark:text-cyan-400">
						Minum
					</span>
					<span class="text-xs text-slate-500 dark:text-slate-400">
						{{ latestMinumLog.created_at ? formatDate(latestMinumLog.created_at) : (latestMinumLog.waktu || '-') }}
					</span>
				</div>
			</div>
    </div>

    <!-- Tabel Data Log Pakan & Minum -->
    <div class="bg-white dark:bg-slate-900/60 border border-slate-200 dark:border-slate-800 rounded-xl overflow-hidden shadow-sm">
      <div class="overflow-x-auto">
        <table class="w-full text-left text-xs border-collapse">
          <thead>
            <tr class="border-b border-slate-200 dark:border-slate-800 bg-slate-50/50 dark:bg-slate-800/40 text-slate-500 dark:text-slate-400 uppercase tracking-wider font-semibold">
              <th class="py-3 px-4">Waktu Pemberian</th>
              <th class="py-3 px-4">Pakan / Minum</th>
              <th class="py-3 px-4">Sisa Pakan (%)</th>
              <th class="py-3 px-4">Mode Kontrol</th>
            </tr>
          </thead>
          <tbody class="divide-y divide-slate-200 dark:divide-slate-800/60 text-slate-700 dark:text-slate-300">
            <tr 
              v-for="(log, index) in filteredPakanLogs" 
              :key="log.id || index"
              class="hover:bg-slate-50/80 dark:hover:bg-slate-800/30 transition-colors"
            >
              <!-- Waktu Pemberian -->
              <td class="py-3 px-4 whitespace-nowrap font-medium text-slate-800 dark:text-slate-200">
                {{ log.created_at ? formatDate(log.created_at) : '-' }}
              </td>

              <!-- Jenis (Pakan / Minum) -->
              <td class="py-3 px-4 whitespace-nowrap">
                <span 
                  :class="[
                    'inline-flex items-center gap-1.5 px-2.5 py-0.5 rounded-md text-[11px] font-bold',
                    (log.jenis_aksi || '').toLowerCase().includes('minum')
                      ? 'bg-cyan-500/10 text-cyan-600 dark:text-cyan-400 border border-cyan-500/20'
                      : 'bg-amber-500/10 text-amber-600 dark:text-amber-400 border border-amber-500/20'
                  ]"
                >
                  {{ log.jenis_aksi || 'Pakan' }}
                </span>
              </td>

              <!-- Sisa Persentase Pakan -->
              <td class="py-3 px-4 whitespace-nowrap font-semibold">
                <span :class="(log.sisa_pakan || 0) < 20 ? 'text-rose-500 font-bold' : ''">
                  {{ log.sisa_pakan !== undefined ? log.sisa_pakan : 0 }} %
                </span>
              </td>

              <!-- Mode Kontrol -->
              <td class="py-3 px-4 whitespace-nowrap">
                <span class="px-2 py-0.5 rounded text-[10px] font-medium bg-slate-100 dark:bg-slate-800 text-slate-600 dark:text-slate-400 border border-slate-200 dark:border-slate-700">
                  {{ log.mode_kontrol || 'Manual' }}
                </span>
              </td>
            </tr>

            <!-- State Kosong -->
            <tr v-if="filteredPakanLogs.length === 0">
              <td colspan="5" class="py-8 text-center text-slate-400">
                Belum ada data log pakan & minum yang terekam.
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
    pakanMinumLogs: {
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
    latestPakanLog() {
      // Hanya ambil data yang jenis_aksi nya benar-benar "pakan" (tidak peduli huruf besar/kecil)
      const pakanLogs = this.pakanMinumLogs.filter(log => {
        const jenis = (log.jenis_aksi || '').toLowerCase();
        return jenis === 'pakan';
      });
      return pakanLogs.length > 0 ? pakanLogs[0] : {};
    },

    latestMinumLog() {
      // Ambil log yang jenis aksinya mengandung kata "minum"
      const minumLogs = this.pakanMinumLogs.filter(log => {
        const jenis = (log.jenis_aksi || '').toLowerCase();
        return jenis === 'minum';
      });
      return minumLogs.length > 0 ? minumLogs[0] : {};
    },

    filteredPakanLogs() {
      if (!Array.isArray(this.pakanMinumLogs)) return [];

      return this.pakanMinumLogs.filter(log => {
        if (log.jenis_aksi === undefined || log.jenis_aksi === null) return false;
        
        const q = this.searchQuery.trim().toLowerCase();
        if (!q) return true; 

        const time = (this.formatDate(log.created_at) || '').toLowerCase();
        const jenis = (log.jenis_aksi || '').toLowerCase();
        const mode = (log.mode_kontrol || '').toLowerCase();
        const sisa_pakan = log.sisa_pakan.toString().toLowerCase();

        return time.includes(q) || jenis.includes(q) || mode.includes(q) || sisa_pakan.includes(q);
      });
    }
  }
}
</script>