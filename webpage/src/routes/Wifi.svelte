<script lang="ts">
    let ssid = $state("");
    let password = $state("");
    let submitStatus = $state("");

    const onSubmit = async () => {
        try {
            const response: Response = await fetch("/wifi", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({ssid, password})
            });

            if (!response.ok) {
                submitStatus = `HTTP error! Status: ${response.status}`;
                return;
            }

            const result = await response.json(); // Parse the JSON response from the server
            console.log("Success:", result);
            submitStatus = "connected";
        } catch (error) {
            console.error("Error:", error);
            submitStatus = `failed to connect: ${(error as Error).message}`;
        }
    };
</script>

<div class="flex h-full items-center justify-center">
    <div class="card w-full max-w-sm bg-base-100 shadow-xl">
        <div class="card-body gap-4">
            <h2 class="card-title">Wi-Fi Configuration</h2>

            <fieldset class="fieldset">
                <label class="fieldset-legend" for="ssid">SSID</label>
                <input
                        bind:value={ssid}
                        class="input-bordered input w-full"
                        id="ssid"
                        placeholder="Network name"
                        type="text"
                />
            </fieldset>

            <fieldset class="fieldset">
                <label class="fieldset-legend" for="password">Password</label>
                <input
                        bind:value={password}
                        class="input-bordered input w-full"
                        id="password"
                        placeholder="••••••••"
                        type="password"
                />
            </fieldset>

            <div class="mt-2 card-actions">
                <button class="btn w-full btn-primary" onclick={onSubmit}>Connect</button>
            </div>

            {#if submitStatus}
                <p
                        class="text-center font-mono text-sm"
                        class:text-success={submitStatus === "connected"}
                        class:text-error={submitStatus !== "connected"}
                >
                    {submitStatus}
                </p>
            {/if}
        </div>
    </div>
</div>
