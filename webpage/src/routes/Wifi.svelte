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

<div>

    <fieldset class="fieldset">
    	<label class="fieldset-legend" for="ssid">Wifi SSID</label>
        <input type="text" bind:value={ssid} placeholder="SSID" id="ssid" />
    </fieldset>

    <fieldset class="fieldset">
    	<label class="fieldset-legend" for="password">Wifi Password</label>
        <input type="password" bind:value={password} placeholder="Password" id="password" />
    </fieldset>

	<button class="btn btn-primary" onclick={onSubmit}>Connect</button>

	<p>Submit Status: {submitStatus}</p>
</div>
