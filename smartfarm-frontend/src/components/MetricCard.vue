<template>
  <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
    <!-- Stat 1: Suhu -->
    <div class="p-4 rounded-xl bg-white dark:bg-slate-900/80 border border-slate-200 dark:border-slate-800 shadow-sm">
      <div class="flex items-center justify-between text-slate-500 dark:text-slate-400 mb-2">
        <span class="text-xs font-medium uppercase tracking-wider">Suhu Kandang</span>
        <span class="p-1.5 bg-orange-500/10 text-orange-500 rounded-md border border-orange-500/20 text-xs">🌡️</span>
      </div>
      <div class="text-2xl font-bold text-slate-900 dark:text-slate-100">
        {{ latestLog.suhu ? latestLog.suhu + ' °C' : '--' }}
      </div>
      <p class="text-[11px] text-slate-500 dark:text-slate-400 mt-1">
        Batas Maksimal: {{ config.batas_suhu || '--' }} °C
      </p>
    </div>

    <!-- Stat 2: Kadar Gas -->
    <div class="p-4 rounded-xl bg-white dark:bg-slate-900/80 border border-slate-200 dark:border-slate-800 shadow-sm">
      <div class="flex items-center justify-between text-slate-500 dark:text-slate-400 mb-2">
        <span class="text-xs font-medium uppercase tracking-wider">Kadar Gas Amonia</span>
        <span class="p-1.5 bg-yellow-500/10 text-yellow-500 rounded-md border border-yellow-500/20 text-xs">⚠️</span>
      </div>
      <div class="text-2xl font-bold text-slate-900 dark:text-slate-100">
          {{ latestLog.gas_ppm !== undefined && latestLog.gas_ppm !== null ? latestLog.gas_ppm + ' PPM' : '--' }}
      </div>
      <p class="text-[11px] text-slate-500 dark:text-slate-400 mt-1">
        Batas Maksimal: {{ config.batas_gas || '--' }} PPM
      </p>
    </div>

    <!-- Stat 3: Status Kipas -->
    <div class="p-4 rounded-xl bg-white dark:bg-slate-900/80 border border-slate-200 dark:border-slate-800 shadow-sm">
      <div class="flex items-center justify-between text-slate-500 dark:text-slate-400 mb-2">
        <span class="text-xs font-medium uppercase tracking-wider">Sistem Ekstraktor Kipas</span>
        <span class="p-1.5 bg-cyan-500/10 text-cyan-500 rounded-md border border-cyan-500/20 text-xs">🌀</span>
      </div>
      <div class="text-2xl font-bold">
        <span 
          :class="(latestLog.status_kipas == 1 || latestLog.status_kipas === 'ON' || latestLog.status_kipas === true) 
            ? 'text-emerald-600 dark:text-emerald-400' 
            : 'text-rose-600 dark:text-rose-400'"
        >
          {{ (latestLog.status_kipas == 1 || latestLog.status_kipas === 'ON' || latestLog.status_kipas === true) ? 'ON' : 'OFF' }}
        </span>
      </div>
      <p class="text-[11px] text-slate-500 dark:text-slate-400 mt-1">Kontrol Otomatis Threshold</p>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    latestLog: {
      type: Object,
      default: () => ({})
    },
    config: {
      type: Object,
      default: () => ({ batas_suhu: 0, batas_gas: 0 })
    }
  }
}
</script>