<script lang="ts">
	import { onMount } from 'svelte';

	let healthCheck = $state('...');

	onMount(async () => {
		const response = await fetch('/health');
		const data = await response.json();
		healthCheck = data.status;
	});
</script>

<div class="flex flex-col items-center justify-center min-h-screen">
	<h1 class="text-4xl font-bold mb-4">Welcome to the ESP32 Webpage</h1>
	<p class="text-lg mb-8">This is a simple Svelte application running on an ESP32.</p>
	<div class="bg-white p-6 rounded-lg shadow-md w-full max-w-md">
		<p class="text-2xl font-semibold mb-4 text-black">Health Status: {healthCheck}</p>
	</div>
</div>