<script lang="ts">
	import { onMount } from 'svelte';

	let healthCheck = $state('...');

	onMount(async () => {
		try {
			const response = await fetch('/health');
			const data = await response.json();
			healthCheck = data.status;
		} catch (error) {
			console.error('Error fetching health status:', error);
			healthCheck = 'Error fetching health status';
		}
	});
</script>

<div class="flex min-h-[calc(100vh-var(--navbar-size))] flex-col items-center justify-center">
	<h1 class="mb-4 text-4xl font-bold">Welcome to the ESP32 Webpage</h1>
	<p class="mb-8 text-lg">This is a simple Svelte application running on an ESP32.</p>
	<div class="w-full max-w-md rounded-lg bg-white p-2 shadow-md">
		<p class="text-md mb-4 font-mono font-semibold text-black">Health Status: {healthCheck}</p>
	</div>
</div>
