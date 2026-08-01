import {createRouter} from "sv-router";
import WifiPage from "./routes/Wifi.svelte";
import Home from "./routes/Home.svelte";

export const {
    p: routerLink,
    navigate,
    isActive,
    route
} = createRouter({
    "/": Home,
    "/wifi": WifiPage
});
