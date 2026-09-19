<template>
  <header class="h-16 border-b border-slate-200 dark:border-slate-800/80 px-6 flex items-center justify-between bg-white/50 dark:bg-slate-900/30 backdrop-blur">
    <!-- SISI KIRI: Hamburger Menu (Mobile) & Live Telemetry Badge -->
    <div class="flex items-center gap-3">
      <!-- Tombol Hamburger (Hanya Tampil di Layar Kecil/Mobile) -->
      <button 
        @click="$emit('toggle-sidebar')" 
        class="md:hidden p-2 text-slate-600 dark:text-slate-300 hover:bg-slate-200/60 dark:hover:bg-slate-800 rounded-lg transition"
        title="Buka Menu"
      >
        <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 6h16M4 12h16M4 18h16"/>
        </svg>
      </button>

      <!-- Badge Status Dinamis (Live / Offline) -->
      <span :class="[
        'inline-flex items-center gap-1.5 px-2.5 py-0.5 rounded-full text-[11px] font-medium border transition-colors',
        isDeviceOnline 
          ? 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border-emerald-500/20' 
          : 'bg-rose-500/10 text-rose-600 dark:text-rose-400 border-rose-500/20'
      ]">
        <span :class="[
          'w-1.5 h-1.5 rounded-full',
          isDeviceOnline ? 'bg-emerald-500 dark:bg-emerald-400 animate-pulse' : 'bg-rose-500 dark:bg-rose-400'
        ]"></span>
        {{ isDeviceOnline ? 'Live Telemetry' : 'Offline' }}
      </span>
    </div>

    <!-- SISI KANAN: Theme Toggle & Refresh -->
    <div class="flex items-center gap-2">
      <!-- TOGGLE LIGHT / DARK MODE SWITCH -->
      <button 
        @click="$emit('toggle-theme')" 
        class="relative inline-flex items-center p-1 bg-slate-200 dark:bg-slate-800 rounded-lg border border-slate-300 dark:border-slate-700 transition duration-200"
        title="Ganti Tema"
      >
        <!-- Opsi Light Mode -->
        <span 
          :class="[
            'px-2.5 py-1 text-xs font-medium rounded-md transition-all duration-200 flex items-center gap-1',
            !isDark 
              ? 'bg-white text-slate-900 shadow-sm font-semibold' 
              : 'text-slate-400 opacity-50 hover:opacity-80'
          ]"
        >
          Light
        </span>

        <!-- Opsi Dark Mode -->
        <span 
          :class="[
            'px-2.5 py-1 text-xs font-medium rounded-md transition-all duration-200 flex items-center gap-1',
            isDark 
              ? 'bg-slate-700 text-slate-100 shadow-sm font-semibold' 
              : 'text-slate-500 opacity-50 hover:opacity-80'
          ]"
        >
          Dark
        </span>
      </button>

      <!-- REFRESH BUTTON -->
      <button 
        @click="$emit('refresh-data')" 
        class="p-2 text-slate-500 dark:text-slate-400 hover:text-slate-800 dark:hover:text-slate-200 hover:bg-slate-200/60 dark:hover:bg-slate-800 rounded-lg transition" 
        title="Refresh Data"
      >
        <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15"/></svg>
      </button>
    </div>
  </header>
</template>

<script>
export default {
  props: {
    isDark: Boolean,
    sensorLogs: {
      type: Array,
      default: () => []
    }
  },
  data() {
    return {
      now: new Date(),
      timerCheck: null
    }
  },
  mounted() {
    // Memperbarui waktu lokal tiap detik untuk kalkulasi selisih waktu real-time
    this.timerCheck = setInterval(() => {
      this.now = new Date()
    }, 1000)
  },
  beforeUnmount() {
    if (this.timerCheck) {
      clearInterval(this.timerCheck)
    }
  },
  computed: {
    isDeviceOnline() {
      if (!this.sensorLogs || this.sensorLogs.length === 0) return false;

      const latestLog = this.sensorLogs[0];
      const logTimeStr = latestLog.updated_at || latestLog.created_at;
      if (!logTimeStr) return false;

      // Ubah spasi ke 'T'
      let formattedStr = logTimeStr.replace(' ', 'T');

      // Jika string belum memiliki penanda zona waktu ('Z' atau '+' atau '-'), tambahkan 'Z' atau biarkan
      if (!formattedStr.endsWith('Z') && !formattedStr.includes('+') && !formattedStr.includes('-', 10)) {
        formattedStr += 'Z';
      }

      const logTime = new Date(formattedStr).getTime();
      const currentTime = this.now.getTime();

      const diffInSeconds = (currentTime - logTime) / 1000;

      // Debugging untuk memastikan angka Diff sudah muncul
      // console.log(`Log Time: ${formattedStr} | Diff: ${diffInSeconds.toFixed(1)} detik`);

      // Toleransi dari -10 hingga 30 detik
      return diffInSeconds >= -10 && diffInSeconds <= 25;
    }
  }
}
</script>