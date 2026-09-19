import { createRouter, createWebHistory } from 'vue-router'
import DashboardView from '../views/DashboardView.vue'
import SettingsView from '../views/SettingsView.vue'
import TelemetrySuhu from '../views/TelemerySuhu.vue'
import TelemetryGas from '../views/TelemetryGas.vue'
import TelemetryPakanMinum from '../views/TelemetryPakanMinum.vue'

const routes = [
  { path: '/', name: 'Dashboard', component: DashboardView },
  { path: '/settings', name: 'Settings', component: SettingsView },
  { path: '/telemetri/suhu', name: 'TelemetrySuhu', component: TelemetrySuhu },
  { path: '/telemetri/gas', name: 'TelemetryGas', component: TelemetryGas },
  { path: '/telemetri/pakan-minum', name: 'TelemetryPakanMinum', component: TelemetryPakanMinum }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

export default router