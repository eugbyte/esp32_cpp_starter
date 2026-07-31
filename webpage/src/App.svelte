<script lang="ts">
	import { Router } from "sv-router";
	import "./layout.css";
	import { Navbar } from "./components";
</script>

<!-- App.svelte -->
<div class="flex h-screen flex-col">
	<div class="shrink-0">
		<Navbar />
	</div>

	<main class="flex-1 overflow-y-auto p-1">
		<Router />
	</main>
</div>
