import { createApp } from 'vue'
import App from './App.vue'
import './style.css'
import axios from 'axios'
import router from './router' // Import router

// axios.defaults.baseURL = 'http://127.0.0.1:8000/api'
axios.defaults.baseURL = 'http://10.10.11.96:8000/api'

const app = createApp(App)
app.use(router) // Pasang router
app.mount('#app')