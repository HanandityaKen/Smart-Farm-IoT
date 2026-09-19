<template>
  <div class="space-y-6">
    <!-- METRIC CARDS -->
    <MetricCard :latestLog="latestLog" :config="config" />

    <!-- GRID GAUGE INDICATOR -->
    <div class="grid grid-cols-1 md:grid-cols-3 gap-5">
      <GaugeCard 
        title="Suhu Ruang"
        :value="latestLog.suhu || 0"
        unit="°C"
        :min="0"
        :max="100"
        color="#f87171" 
      />
      <GaugeCard 
        title="Konsentrasi Gas Metana"
        :value="latestLog.gas_ppm || 0"
        unit="ppm"
        :min="0"
        :max="10000"
        color="#84cc16" 
      />
      <GaugeCard 
        title="Volume Pakan"
        :value="latestLog.sisa_pakan || 0"
        unit="%"
        :min="0"
        :max="100"
        color="#3b82f6"
        :isFullCircle="true" 
      />
    </div>

		<!-- QUICK CONTROLS (BUTTON & SWITCH BLYNK) -->
    <QuickControls :pakanMinumLogs="pakanMinumLogs" :formatDate="formatDate" :latestLog="latestLog" :suhuLogs="suhuLogs"/>

    <!-- TABEL DATA TELEMETRI -->
    <div class="rounded-xl bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 shadow-sm overflow-hidden">
      <div class="px-5 py-4 border-b border-slate-200 dark:border-slate-800 flex items-center justify-between">
        <h3 class="text-sm font-semibold text-slate-800 dark:text-slate-200">Riwayat Telemetri Masuk</h3>
        <span class="text-xs text-slate-500 dark:text-slate-400">Update setiap 5 detik</span>
      </div>

      <div class="overflow-x-auto">
        <table class="w-full text-left text-xs border-collapse">
          <thead>
            <tr class="border-b border-slate-200 dark:border-slate-800 bg-slate-50 dark:bg-slate-950/40 text-slate-500 dark:text-slate-400 font-medium">
              <th class="p-3.5 pl-5">Timestamp</th>
              <th class="p-3.5">Suhu (°C)</th>
              <th class="p-3.5">Gas PPM</th>
              <th class="p-3.5">Sisa Pakan (%)</th>
              <th class="p-3.5 pr-5 text-right">Status Kipas</th>
            </tr>
          </thead>
          <tbody class="divide-y divide-slate-200 dark:divide-slate-800/60">
            <tr v-for="log in sensorLogs" :key="log.id" class="hover:bg-slate-50 dark:hover:bg-slate-800/40 transition">
              <td class="p-3.5 pl-5 font-mono text-slate-500 dark:text-slate-400">{{ formatDate(log.updated_at || log.created_at) }}</td>
              <td class="p-3.5 font-semibold text-slate-800 dark:text-slate-200">{{ log.suhu }} °C</td>
              <td class="p-3.5 font-semibold text-slate-800 dark:text-slate-200">{{ log.gas_ppm }} PPM</td>
              <td class="p-3.5 text-cyan-600 dark:text-cyan-400 font-medium">{{ Number(log.sisa_pakan).toFixed(1) }} %</td>
              <td class="p-3.5 pr-5 text-right">
                <span 
                  :class="[
                    'inline-flex items-center gap-1 px-2 py-0.5 rounded text-[11px] font-medium border',
                    log.status_kipas === 'ON' 
                      ? 'bg-emerald-500/10 text-emerald-600 dark:text-emerald-400 border-emerald-500/20' 
                      : 'bg-slate-100 dark:bg-slate-800 text-slate-500 dark:text-slate-400 border-slate-300 dark:border-slate-700'
                  ]"
                >
                <span :class="['w-1.5 h-1.5 rounded-full', (log.status_kipas == 1 || log.status_kipas === 'ON' || log.status_kipas === true) ? 'bg-emerald-500' : 'bg-slate-400']"></span>
                  {{ (log.status_kipas == 1 || log.status_kipas === 'ON' || log.status_kipas === true) ? 'ON' : 'OFF' }}
                </span>
              </td>
            </tr>
            <tr v-if="sensorLogs.length === 0">
              <td colspan="5" class="p-8 text-center text-slate-500 italic">
                Belum ada sinyal telemetri yang terkirim dari ESP32
              </td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
  </div>
</template>

<script>
import GaugeCard from '../components/GaugeCard.vue'
import MetricCard from '../components/MetricCard.vue'
import QuickControls from '../components/QuickControls.vue'

export default {
  components: { 
    GaugeCard, 
    MetricCard,
    QuickControls
  },
  props: {
    latestLog: Object,
    pakanMinumLogs: Array,
    config: Object,
    sensorLogs: Array,
    suhuLogs: Array,
    formatDate: Function
  }
}
</script>