<template>
  <div class="flex min-h-screen bg-slate-50 dark:bg-slate-950 text-slate-800 dark:text-slate-100 font-sans antialiased transition-colors duration-200">
    
    <!-- SIDEBAR -->
    <Sidebar 
      :isOpen="isSidebarOpen" 
      :sensorLogs="sensorLogs"
      @close-sidebar="isSidebarOpen = false" 
    />

    <!-- MAIN CONTENT AREA -->
    <div class="flex-1 flex flex-col min-w-0">
      
      <!-- HEADER -->
      <Header
        :sensorLogs="sensorLogs" 
        :isDark="isDark" 
        @toggle-sidebar="isSidebarOpen = !isSidebarOpen"
        @toggle-theme="toggleTheme" 
        @refresh-data="fetchAll" 
      />

      <!-- PAGE BODY (Halaman berubah dinamis) -->
      <main class="p-6 overflow-y-auto">
        <router-view 
          :latestLog="latestLog" 
          :sensorLogs="sensorLogs" 
          :suhuLogs="suhuLogs"
          :gasLogs="gasLogs"
          :pakanMinumLogs="pakanMinumLogs"
          :formatDate="formatDate" 
          :config="config"
          :isLoading="isLoading"
        />
      </main>
    </div>
  </div>
</template>

<script>
import axios from 'axios'
import Sidebar from './components/Sidebar.vue'
import Header from './components/Header.vue'

export default {
  components: { Sidebar, Header },
  data() {
    return {
      sensorLogs: [],
      gasLogs: [],
      suhuLogs: [],
      pakanMinumLogs: [],
      config: {},
      isDark: true,
      isSidebarOpen: false,
      isLoading: true
    }
  },
  computed: {
    latestLog() {
      return this.sensorLogs.length > 0 ? this.sensorLogs[0] : {}
    },
  },
  async mounted() {
    const savedTheme = localStorage.getItem('theme')
    if (savedTheme) {
      this.isDark = savedTheme === 'dark'
    } else {
      this.isDark = window.matchMedia('(prefers-color-scheme: dark)').matches
    }
    this.applyTheme()

    this.isLoading = true
    await Promise.all([this.fetchConfig(), this.fetchAll()])
    this.isLoading = false
    
    setInterval(() => {
      this.fetchAll()
    }, 5000)
  },
  methods: {
    toggleTheme() {
      this.isDark = !this.isDark
      localStorage.setItem('theme', this.isDark ? 'dark' : 'light')
      this.applyTheme()
    },
    applyTheme() {
      if (this.isDark) {
        document.documentElement.classList.add('dark')
      } else {
        document.documentElement.classList.remove('dark')
      }
    },
    async fetchAll() {
      await Promise.all([this.fetchLogs(), this.fetchGasLogs(), this.fetchSuhuLogs(), this.fetchPakanMinumLogs()])
    },
    async fetchLogs() {
      try {
        const res = await axios.get('/sensor')
        if (res.data && res.data.data) {
          this.sensorLogs = res.data.data
        } else if (Array.isArray(res.data)) {
          this.sensorLogs = res.data
        }
      } catch (err) {
        console.error('Gagal mengambil log sensor:', err)
      }
    },
    async fetchSuhuLogs() {
      try {
        const res = await axios.get('/telemetri/suhu')
        if (res.data && res.data.data) {
          this.suhuLogs = res.data.data
        } else if (Array.isArray(res.data)) {
          this.suhuLogs = res.data
        }
      } catch (err) {
        console.error('Gagal mengambil log suhu:', err)
      }
    },
    async fetchGasLogs() {
      try {
        const res = await axios.get('/telemetri/gas')
        if (res.data && res.data.data) {
          this.gasLogs = res.data.data
        } else if (Array.isArray(res.data)) {
          this.gasLogs = res.data
        }
      } catch (err) {
        console.error('Gagal mengambil log gas:', err)
      }
    },
    async fetchPakanMinumLogs() {
      try {
        const res = await axios.get('/telemetri/pakan-minum')
        if (res.data && res.data.data) {
          this.pakanMinumLogs = res.data.data
        } else if (Array.isArray(res.data)) {
          this.pakanMinumLogs = res.data
        }
      } catch (err) {
        console.error('Gagal mengambil log pakan/minum:', err)
      }
    },
    async fetchConfig() {
      try {
        const res = await axios.get('/konfigurasi')
        if (res.data && res.data.data) {
          this.config = res.data.data
        } else if (res.data) {
          this.config = res.data
        }
      } catch (err) {
        console.error('Gagal mengambil config:', err)
      }
    },
    formatDate(dateStr) {
      if (!dateStr) return '-'
      
      const date = new Date(dateStr)
      if (isNaN(date.getTime())) return dateStr

      const day = String(date.getDate()).padStart(2, '0')
      const month = String(date.getMonth() + 1).padStart(2, '0')
      const year = date.getFullYear()

      const hours = String(date.getHours()).padStart(2, '0')
      const minutes = String(date.getMinutes()).padStart(2, '0')

      return `${day}-${month}-${year}, ${hours}:${minutes} WIB`
    }
  }
}
</script>