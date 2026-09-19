<template>
  <!-- Pembungkus Utama: Tanpa class flex/w-64 agar tidak memakan tempat di desktop -->
  <div>
    <!-- Backdrop Overlay (Hanya Tampak di Mobile saat Sidebar Terbuka) -->
    <div 
      v-if="isOpen" 
      @click="$emit('close-sidebar')" 
      class="fixed inset-0 bg-slate-900/50 backdrop-blur-sm z-40 md:hidden transition-opacity"
    ></div>

    <!-- Sidebar Element -->
    <aside 
      :class="[
        'w-64 h-screen bg-white dark:bg-slate-900 border-r border-slate-200 dark:border-slate-800 flex flex-col justify-between z-50 transition-transform duration-300 ease-in-out shrink-0',
        'fixed top-0 left-0 md:sticky',
        isOpen ? 'translate-x-0' : '-translate-x-full md:translate-x-0'
      ]"
    >
      <div class="flex-1 overflow-y-auto">
        <!-- Logo Brand -->
        <div class="h-16 flex items-center justify-between px-6 border-b border-slate-200 dark:border-slate-800/80">
          <div class="flex items-center gap-3">
            <img 
              src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTHBgN7CxN8j0xCssdF-6061yjZEtanW5mvLUxt3H0uKg&s=10" 
              alt="Logo SmartFarm" 
              class="h-8 w-8 object-cover rounded-lg border border-slate-200 dark:border-slate-700 shadow-sm shrink-0"
            />
            <div>
              <h2 class="font-semibold text-sm text-slate-800 dark:text-slate-100 leading-tight">SmartFarm</h2>
              <p class="text-[11px] text-slate-500 dark:text-slate-400">Enterprise Console</p>
            </div>
          </div>

          <!-- Tombol Close (X) Khusus Mobile -->
          <button 
            @click="$emit('close-sidebar')" 
            class="md:hidden p-1.5 text-slate-500 hover:text-slate-800 dark:text-slate-400 dark:hover:text-slate-200 rounded-lg"
          >
            <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12"/>
            </svg>
          </button>
        </div>

        <!-- Navigation Links -->
        <nav class="p-4 space-y-1">
          <router-link 
            to="/" 
            @click="$emit('close-sidebar')"
            class="flex items-center gap-3 px-3 py-2 text-xs font-medium text-slate-600 dark:text-slate-400 hover:text-slate-900 dark:hover:text-slate-200 hover:bg-slate-100 dark:hover:bg-slate-800/50 rounded-lg transition"
            active-class="!text-emerald-600 dark:!text-emerald-400 bg-emerald-500/10 border border-emerald-500/20 font-semibold"
            exact-active-class="!text-emerald-600 dark:!text-emerald-400 bg-emerald-500/10 border border-emerald-500/20 font-semibold"
          >
            <svg class="w-4 h-4 text-current" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 12l2-2m0 0l7-7 7 7M5 10v10a1 1 0 001 1h3m10-11l2 2m-2-2v10a1 1 0 01-1 1h-3m-6 0a1 1 0 00-1-1v-4a1 1 0 011-1h2a1 1 0 011 1v4a1 1 0 00-1 1m-6 0h6"/></svg>
            Dashboard
          </router-link>

          <!-- Group Menu: Riwayat Telemetri -->
          <div>
            <button 
              @click="isTelemetriOpen = !isTelemetriOpen"
              class="w-full flex items-center justify-between px-3 py-2 text-xs font-medium text-slate-600 dark:text-slate-400 hover:text-slate-900 dark:hover:text-slate-200 hover:bg-slate-100 dark:hover:bg-slate-800/50 rounded-lg transition"
            >
              <div class="flex items-center gap-3">
                <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z"/></svg>
                <span>Riwayat Telemetri</span>
              </div>
              <svg 
                :class="['w-3.5 h-3.5 transition-transform duration-200', isTelemetriOpen ? 'rotate-180' : '']" 
                fill="none" stroke="currentColor" viewBox="0 0 24 24"
              >
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 9l-7 7-7-7"/>
              </svg>
            </button>

            <!-- Sub-Menu Items -->
            <div v-show="isTelemetriOpen" class="mt-1 pl-4 space-y-1">
              <router-link 
                to="/telemetri/suhu" 
                @click="$emit('close-sidebar')"
                class="flex items-center gap-2.5 px-3 py-2 text-xs font-medium text-slate-500 dark:text-slate-400 hover:text-slate-800 dark:hover:text-slate-200 hover:bg-slate-100 dark:hover:bg-slate-800/40 rounded-md transition group"
                active-class="!text-emerald-600 dark:!text-emerald-400 bg-emerald-500/10 font-semibold"
              >
                <svg class="w-4 h-4 text-orange-500/80 group-hover:text-orange-500 transition-colors" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24">
                  <path d="M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z"/>
                </svg>
                Log Suhu Kandang
              </router-link>

              <router-link 
                to="/telemetri/gas" 
                @click="$emit('close-sidebar')"
                class="flex items-center gap-2.5 px-3 py-2 text-xs font-medium text-slate-500 dark:text-slate-400 hover:text-slate-800 dark:hover:text-slate-200 hover:bg-slate-100 dark:hover:bg-slate-800/40 rounded-md transition group"
                active-class="!text-emerald-600 dark:!text-emerald-400 bg-emerald-500/10 font-semibold"
              >
                <svg class="w-4 h-4 text-amber-500/80 group-hover:text-amber-500 transition-colors" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z"/>
                </svg>
                Log Gas Amonia & Metana
              </router-link>

              <router-link 
                to="/telemetri/pakan-minum" 
                @click="$emit('close-sidebar')"
                class="flex items-center gap-2.5 px-3 py-2 text-xs font-medium text-slate-500 dark:text-slate-400 hover:text-slate-800 dark:hover:text-slate-200 hover:bg-slate-100 dark:hover:bg-slate-800/40 rounded-md transition group"
                active-class="!text-emerald-600 dark:!text-emerald-400 bg-emerald-500/10 font-semibold"
              >
                <svg class="w-4 h-4 text-cyan-500/80 group-hover:text-cyan-500 transition-colors" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24">
                  <path d="m16 2-2.3 2.3a3 3 0 0 0 0 4.2l1.8 1.8a3 3 0 0 0 4.2 0L22 8"/>
                  <path d="M15 15 3.3 3.3a4.2 4.2 0 0 0 0 6l7.3 7.3c.7.7 2 .7 2.8 0L15 15Zm0 0 7 7"/>
                  <path d="m2.1 21.9 8.8-8.8"/>
                </svg>
                Log Pakan & Minum
              </router-link>
            </div>
          </div>

          <router-link 
            to="/settings" 
            @click="$emit('close-sidebar')"
            class="flex items-center gap-3 px-3 py-2 text-xs font-medium text-slate-600 dark:text-slate-400 hover:text-slate-900 dark:hover:text-slate-200 hover:bg-slate-100 dark:hover:bg-slate-800/50 rounded-lg transition"
            active-class="!text-emerald-600 dark:!text-emerald-400 bg-emerald-500/10 border border-emerald-500/20 font-semibold"
          >
            <svg class="w-4 h-4 text-current" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24">
              <path d="M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.39a2 2 0 0 0-.73-2.73l-.15-.08a2 2 0 0 1-1-1.74v-.5a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z"/>
              <circle cx="12" cy="12" r="3"/>
            </svg>
            Konfigurasi Sistem
          </router-link>
        </nav>
      </div>

      <!-- Footer User Info -->
      <div class="p-4 border-t border-slate-200 dark:border-slate-800/80 flex items-center justify-between shrink-0 bg-white dark:bg-slate-900">
        <div class="flex items-center gap-3">
          <div class="w-8 h-8 rounded-full bg-slate-200 dark:bg-slate-800 border border-slate-300 dark:border-slate-700 flex items-center justify-center font-bold text-xs text-slate-700 dark:text-slate-300">
            SF
          </div>
          <div class="text-xs">
            <p class="font-medium text-slate-800 dark:text-slate-200">Operator Stasiun</p>
            <p class="text-[10px] text-emerald-500 font-semibold flex items-center gap-1">
              <span class="w-1.5 h-1.5 rounded-full bg-emerald-500 animate-pulse"></span>
              Online
            </p>
          </div>
        </div>
      </div>
    </aside>
  </div>
</template>

<script>
export default {
  props: {
    isOpen: {
      type: Boolean,
      default: false
    }
  },
  data() {
    return {
      isTelemetriOpen: false,
    };
  },
  mounted() {
    if (this.$route && this.$route.path.startsWith('/telemetri')) {
      this.isTelemetriOpen = true;
    }
  },
  watch: {
    '$route'(to) {
      if (to.path.startsWith('/telemetri')) {
        this.isTelemetriOpen = true;
      }
    }
  }
};
</script>