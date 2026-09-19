<template>
  <div class="grid grid-cols-1 md:grid-cols-3 gap-5">
    
    <!-- KELOMPOK 1: KONTROL PAKAN & POMPA -->
    <div class="p-5 rounded-2xl bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 shadow-sm space-y-4">
      <h3 class="text-xs font-bold text-slate-500 dark:text-slate-400 uppercase tracking-wider">Aksi Manual Pakan</h3>
      
      <!-- Button Pakan -->
      <div>
        <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Button Pakan</label>
        <button 
          @click="triggerPakan"
          :disabled="isPakanLoading"
          class="w-full py-2.5 px-4 rounded-lg font-bold text-xs transition duration-200 shadow-sm flex items-center justify-center gap-2 bg-emerald-600 hover:bg-emerald-500 text-white active:scale-95 disabled:opacity-70 disabled:cursor-not-allowed disabled:active:scale-100"
        >
          <!-- Animasi Spinner SVG saat Loading -->
          <svg v-if="isPakanLoading" class="animate-spin h-4 w-4 text-white" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
            <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
            <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
          </svg>
          <span>{{ isPakanLoading ? 'MEMPROSES...' : 'BERI PAKAN' }}</span>
        </button>
        <p class="text-[11px] text-slate-400 mt-1">Terakhir: <span class="font-semibold text-slate-600 dark:text-slate-300">{{ formatDate(latestPakanLog.created_at) || '--:-- WIB' }}</span></p>
      </div>

      <!-- Button Pompa Air -->
      <div class="pt-2 border-t border-slate-100 dark:border-slate-800">
        <label class="block text-xs font-semibold text-slate-700 dark:text-slate-300 mb-1.5">Button Pompa Air</label>
        <button 
          @click="triggerPompa"
          :disabled="isPompaLoading"
          class="w-full py-2.5 px-4 rounded-lg font-bold text-xs transition duration-200 shadow-sm flex items-center justify-center gap-2 bg-cyan-600 hover:bg-cyan-500 text-white active:scale-95 disabled:opacity-70 disabled:cursor-not-allowed disabled:active:scale-100"
        >
          <!-- Animasi Spinner SVG saat Loading -->
          <svg v-if="isPompaLoading" class="animate-spin h-4 w-4 text-white" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
            <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
            <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
          </svg>
          <span>{{ isPompaLoading ? 'MEMPROSES...' : 'BERI MINUM' }}</span>
        </button>
        <p class="text-[11px] text-slate-400 mt-1">Terakhir: <span class="font-semibold text-slate-600 dark:text-slate-300">{{ formatDate(latestMinumLog.created_at) || '--:-- WIB' }}</span></p>
      </div>
    </div>

    <!-- KELOMPOK 2: SWITCH LAMPU DALAM & LUAR -->
    <div class="p-5 rounded-2xl bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 shadow-sm space-y-4">
      <h3 class="text-xs font-bold text-slate-500 dark:text-slate-400 uppercase tracking-wider">Kontrol Pencahayaan</h3>

      <!-- Switch Lampu Dalam -->
      <div class="flex items-center justify-between pb-3 border-b border-slate-100 dark:border-slate-800">
        <div>
          <p class="text-xs font-semibold text-slate-800 dark:text-slate-200">Lampu Dalam</p>
          <span class="text-[11px] font-bold" :class="controls.lampu_dalam ? 'text-emerald-500' : 'text-slate-400'">
            {{ controls.lampu_dalam ? 'ON' : 'OFF' }}
          </span>
        </div>
        <button 
          @click="toggleSwitch('lampu_dalam')" 
          :class="[
            'relative inline-flex h-6 w-11 flex-shrink-0 cursor-pointer rounded-full border-2 border-transparent transition-colors duration-200 ease-in-out focus:outline-none',
            controls.lampu_dalam ? 'bg-emerald-500' : 'bg-slate-300 dark:bg-slate-700'
          ]"
        >
          <span 
            :class="[
              'pointer-events-none inline-block h-5 w-5 transform rounded-full bg-white shadow ring-0 transition duration-200 ease-in-out',
              controls.lampu_dalam ? 'translate-x-5' : 'translate-x-0'
            ]"
          ></span>
        </button>
      </div>

      <!-- Switch Lampu Luar -->
      <div class="flex items-center justify-between">
        <div>
          <p class="text-xs font-semibold text-slate-800 dark:text-slate-200">Lampu Luar</p>
          <span class="text-[11px] font-bold" :class="controls.lampu_luar ? 'text-emerald-500' : 'text-slate-400'">
            {{ controls.lampu_luar ? 'ON' : 'OFF' }}
          </span>
        </div>
        <button 
          @click="toggleSwitch('lampu_luar')" 
          :class="[
            'relative inline-flex h-6 w-11 flex-shrink-0 cursor-pointer rounded-full border-2 border-transparent transition-colors duration-200 ease-in-out focus:outline-none',
            controls.lampu_luar ? 'bg-emerald-500' : 'bg-slate-300 dark:bg-slate-700'
          ]"
        >
          <span 
            :class="[
              'pointer-events-none inline-block h-5 w-5 transform rounded-full bg-white shadow ring-0 transition duration-200 ease-in-out',
              controls.lampu_luar ? 'translate-x-5' : 'translate-x-0'
            ]"
          ></span>
        </button>
      </div>
    </div>

    <!-- KELOMPOK 3: SWITCH KIPAS EKSTRAKTOR -->
    <div class="p-5 rounded-2xl bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 shadow-sm flex flex-col justify-between">
      <div>
        <h3 class="text-xs font-bold text-slate-500 dark:text-slate-400 uppercase tracking-wider mb-4">Kontrol Ventilasi</h3>
        
        <!-- Switch Kipas -->
        <div class="flex items-center justify-between">
          <div>
            <p class="text-xs font-semibold text-slate-800 dark:text-slate-200">Switch Kipas Ekstraktor</p>
            <span class="text-[11px] font-bold" :class="controls.kipas ? 'text-emerald-500' : 'text-slate-400'">
              {{ isAutomaticMode ? 'ON (AUTOMATIC)' : (controls.kipas ? 'ON (MANUAL)' : 'OFF (AUTOMATIC)') }}
            </span>
          </div>
          <button 
            @click="toggleSwitch('kipas')" 
            :disabled="isAutomaticMode"
            :class="[
              'relative inline-flex h-6 w-11 flex-shrink-0 rounded-full border-2 border-transparent transition-colors duration-200 ease-in-out focus:outline-none',
              isAutomaticMode ? 'cursor-not-allowed opacity-75' : 'cursor-pointer',
              controls.kipas ? 'bg-emerald-500' : 'bg-slate-300 dark:bg-slate-700'
            ]"
          >
            <span 
              :class="[
                'pointer-events-none inline-block h-5 w-5 transform rounded-full bg-white shadow ring-0 transition duration-200 ease-in-out',
                controls.kipas ? 'translate-x-5' : 'translate-x-0'
              ]"
            ></span>
          </button>
        </div>
      </div>
    </div>

  </div>
</template>

<script>
import axios from 'axios'

export default {
  props: {
    latestLog: {
      type: Object,
      default: () => ({})
    },
    suhuLogs: {
      type: Array,
      default: () => []
    },
    pakanMinumLogs: {
      type: Array,
      default: () => []
    },
    sensorLogs: {
      type: Array,
      default: () => []
    },
    formatDate: {
      type: Function,
      default: (val) => val
    }
  },
  data() {
    return {
      controls: {
        lampu_dalam: false,
        lampu_luar: false,
        kipas: false,
        trigger_pakan: false,
        trigger_pompa: false,
      },
      isAutomaticMode: false,
      isPakanLoading: false,
      isPompaLoading: false,
      pollingInterval: null
    }
  },
  computed: {
    currentSuhuLog() {
      if (this.latestLog && Object.keys(this.latestLog).length > 0) {
        return this.latestLog;
      }
      return this.suhuLogs.length > 0 ? this.suhuLogs[0] : {};
    },
    latestPakanLog() {
      const pakanLogs = this.pakanMinumLogs.filter(log => {
        const jenis = (log.jenis_aksi || '').toLowerCase();
        return jenis === 'pakan';
      });
      return pakanLogs.length > 0 ? pakanLogs[0] : {};
    },
    latestMinumLog() {
      const minumLogs = this.pakanMinumLogs.filter(log => {
        const jenis = (log.jenis_aksi || '').toLowerCase();
        return jenis === 'minum';
      });
      return minumLogs.length > 0 ? minumLogs[0] : {};
    },
  },
  watch: {
    currentSuhuLog: {
      handler(newLog) {
        if (!newLog) return;

        const statusStr = String(newLog.status_kipas || '').trim().toUpperCase();
        const modeStr = String(newLog.mode_kontrol || '').trim().toLowerCase();

        const isKipasOn = statusStr === 'ON' || statusStr === '1' || newLog.status_kipas === true;
        const isModeAuto = modeStr === 'otomatis' || modeStr === 'automatic';
        const isModeManual = modeStr === 'manual';

        // Kipas dianggap otomatis HANYA jika mode_kontrol eksplisit 'Otomatis' dan Kipas sedang ON
        if (isModeAuto && isKipasOn && !isModeManual) {
          this.isAutomaticMode = true;
          this.controls.kipas = true;
        } else {
          // Jika mode manual (atau suhu normal), lepaskan status disable
          this.isAutomaticMode = false;
          this.fetchControls();
        }
      },
      deep: true,
      immediate: true
    }
  },
  mounted() {
    this.fetchControls()

    this.pollingInterval = setInterval(() => {
      this.fetchControls()
    }, 3000)
  },
  beforeUnmount() {
    if (this.pollingInterval) clearInterval(this.pollingInterval)
  },
  methods: {
    async fetchControls() {
      try {
        const res = await axios.get('/kontrol')
        if (res.data) {
          const isManualKipasOn = Boolean(res.data.kipas == 1 || res.data.kipas === true);

          this.controls = {
            ...this.controls,
            lampu_dalam: Boolean(res.data.lampu_dalam == 1 || res.data.lampu_dalam === true),
            lampu_luar: Boolean(res.data.lampu_luar == 1 || res.data.lampu_luar === true),
            kipas: this.isAutomaticMode ? true : isManualKipasOn,
            trigger_pakan: Boolean(res.data.trigger_pakan == 1 || res.data.trigger_pakan === true),
            trigger_pompa: Boolean(res.data.trigger_pompa == 1 || res.data.trigger_pompa === true),
          }
        }
      } catch (err) {
        console.error('Gagal mengambil status kontrol:', err)
      }
    },
    
    async toggleSwitch(device) {
      if (device === 'kipas' && this.isAutomaticMode) return;

      this.controls[device] = !this.controls[device]
      
      try {
        await axios.patch('/kontrol', {
          [device]: this.controls[device]
        })
      } catch (err) {
        console.error(`Gagal mengupdate ${device}:`, err)
        this.controls[device] = !this.controls[device]
      }
    },

    async triggerPakan() {
      if (this.isPakanLoading) return
      this.isPakanLoading = true
      
      try {
        if (this.pollingInterval) clearInterval(this.pollingInterval);

        await axios.patch('/kontrol', { trigger_pakan: true })
      } catch (err) {
        console.error('Gagal memicu pakan:', err)
      } finally {
        setTimeout(() => {
          this.isPakanLoading = false;
          this.pollingInterval = setInterval(() => { this.fetchControls() }, 5000);
        }, 2000);
      }
    },

    async triggerPompa() {
      if (this.isPompaLoading) return
      this.isPompaLoading = true
      try {
        if (this.pollingInterval) clearInterval(this.pollingInterval)

        await axios.patch('/kontrol', { trigger_pompa: true })
      } catch (err) {
        console.error('Gagal memicu pompa:', err)
      } finally {
        setTimeout(() => {
          this.isPompaLoading = false
          this.pollingInterval = setInterval(() => { this.fetchControls() }, 5000);
        }, 2000)
      }
    }
  }
}
</script>