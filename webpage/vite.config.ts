import {svelte} from "@sveltejs/vite-plugin-svelte";
import {defineConfig} from "vite";
import tailwindcss from "@tailwindcss/vite";
import {resolve} from "path";

export default defineConfig({
    plugins: [tailwindcss(), svelte()],
    resolve: {
        alias: {
            "~": resolve(__dirname, "src")
        }
    }
});
