<script lang="ts">
	let ssid = $state('');
	let password = $state('');
	let submitStatus = $state('');

	const onSubmit = async () => {
		try {
			const response: Response = await fetch('/wifi', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ ssid, password })
			});

			if (!response.ok) {
				submitStatus = `HTTP error! Status: ${response.status}`;
				return;
			}

			const result = await response.json(); // Parse the JSON response from the server
			console.log('Success:', result);
			submitStatus = 'connected';
		} catch (error) {
			console.error('Error:', error);
			submitStatus = `failed to connect: ${(error as Error).message}`;
		}
	};
</script>

<div class="flex items-center justify-center min-h-[calc(100vh-var(--navbar-size))]">
	<div class="card bg-base-100 shadow-xl w-full max-w-sm">
		<div class="card-body gap-4">
			<h2 class="card-title">Wi-Fi Configuration</h2>

			<fieldset class="fieldset">
				<label class="fieldset-legend" for="ssid">SSID</label>
				<input
					type="text"
					bind:value={ssid}
					placeholder="Network name"
					id="ssid"
					class="input input-bordered w-full"
				/>
			</fieldset>

			<fieldset class="fieldset">
				<label class="fieldset-legend" for="password">Password</label>
				<input
					type="password"
					bind:value={password}
					placeholder="••••••••"
					id="password"
					class="input input-bordered w-full"
				/>
			</fieldset>

			<div class="card-actions mt-2">
				<button class="btn btn-primary w-full" onclick={onSubmit}>Connect</button>
			</div>

			{#if submitStatus}
				<p
					class="text-sm text-center font-mono"
					class:text-success={submitStatus === 'connected'}
					class:text-error={submitStatus !== 'connected'}
				>
					{submitStatus}
				</p>
			{/if}
		</div>
	</div>
</div>
