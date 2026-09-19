<template>
  <div class="p-5 rounded-2xl bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800/80 shadow-sm flex flex-col items-center justify-between min-w-[200px]">
    <!-- Judul Indikator -->
    <h3 class="text-xs font-semibold text-slate-600 dark:text-slate-300 self-start mb-2">
      {{ title }}
    </h3>

    <!-- SVG Gauge -->
    <div class="relative flex items-center justify-center my-2">
      <!-- Tampilan Setengah Lingkaran (Semi Arc) -->
      <svg v-if="!isFullCircle" :width="size" :height="size * 0.65" class="overflow-visible">
        <!-- Track Background (Abu-abu) -->
        <path
          :d="semiArcPath"
          fill="none"
          class="stroke-slate-200 dark:stroke-slate-800"
          :stroke-width="strokeWidth"
          stroke-linecap="round"
        />
        <!-- Progress Bar (Warna Dinamis) -->
        <path
          v-if="percentage > 0"
          :d="semiArcPath"
          fill="none"
          :stroke="color"
          :stroke-width="strokeWidth"
          stroke-linecap="round"
          :stroke-dasharray="semiArcLength"
          :stroke-dashoffset="semiArcOffset"
          class="transition-all duration-700 ease-out"
        />
      </svg>

      <!-- Tampilan Lingkaran Penuh (Volume Pakan) -->
      <svg v-else :width="size" :height="size" class="transform -rotate-90">
        <circle
          :cx="size / 2"
          :cy="size / 2"
          :r="radius"
          fill="transparent"
          class="stroke-slate-200 dark:stroke-slate-800"
          :stroke-width="strokeWidth"
        />
        <circle
          v-if="percentage > 0"
          :cx="size / 2"
          :cy="size / 2"
          :r="radius"
          fill="transparent"
          :stroke="color"
          :stroke-width="strokeWidth"
          :stroke-dasharray="fullCircumference"
          :stroke-dashoffset="fullDashOffset"
          stroke-linecap="round"
          class="transition-all duration-700 ease-out"
        />
      </svg>

      <!-- Teks Angka Sensor di Tengah -->
      <div 
        class="absolute flex items-baseline justify-center text-slate-800 dark:text-slate-100" 
        :class="isFullCircle ? 'top-1/2 -translate-y-1/2' : 'bottom-2'"
      >
        <span class="text-2xl font-extrabold tracking-tight">{{ Number(value).toFixed(1) }}</span>
        <span class="text-xs font-medium text-slate-500 dark:text-slate-400 ml-0.5">{{ unit }}</span>
      </div>
    </div>

    <!-- Label Min & Max Nilai -->
    <div v-if="!isFullCircle" class="w-full flex justify-between px-3 text-[11px] font-medium text-slate-400 dark:text-slate-500">
      <span>{{ min }}</span>
      <span>{{ max.toLocaleString() }}</span>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    title: String,
    value: { type: [Number, String], default: 0 },
    unit: String,
    min: { type: Number, default: 0 },
    max: { type: Number, default: 100 },
    color: { type: String, default: '#ef4444' },
    isFullCircle: { type: Boolean, default: false },
    size: { type: Number, default: 160 },
    strokeWidth: { type: Number, default: 14 }
  },
  computed: {
    radius() {
      return (this.size - this.strokeWidth) / 2
    },
    percentage() {
      const numericVal = parseFloat(this.value) || 0
      const clampedValue = Math.min(Math.max(numericVal, this.min), this.max)
      return (clampedValue - this.min) / (this.max - this.min)
    },
    // Logika Setengah Lingkaran (Path Arc)
    semiArcPath() {
      const cx = this.size / 2
      const cy = this.size / 2
      const r = this.radius
      // Titik Awal (Kiri Bawah) & Titik Akhir (Kanan Bawah)
      const startX = cx - r * Math.cos(Math.PI / 6)
      const startY = cy + r * Math.sin(Math.PI / 6)
      const endX = cx + r * Math.cos(Math.PI / 6)
      const endY = cy + r * Math.sin(Math.PI / 6)
      
      return `M ${startX} ${startY} A ${r} ${r} 0 1 1 ${endX} ${endY}`
    },
    semiArcLength() {
      // Panjang busur 240 derajat
      return (240 / 360) * 2 * Math.PI * this.radius
    },
    semiArcOffset() {
      return this.semiArcLength * (1 - this.percentage)
    },
    // Logika Lingkaran Penuh
    fullCircumference() {
      return 2 * Math.PI * this.radius
    },
    fullDashOffset() {
      return this.fullCircumference * (1 - this.percentage)
    }
  }
}
</script>