<script lang="ts">
    import {routerLink} from "~/router";
</script>

<div class="navbar bg-base-100 shadow-sm">
    <div class="navbar-start">
        <div class="dropdown">
            <div class="btn btn-ghost lg:hidden" role="button" tabindex="0">
                <svg
                        class="h-5 w-5"
                        fill="none"
                        stroke="currentColor"
                        viewBox="0 0 24 24"
                        xmlns="http://www.w3.org/2000/svg"
                >
                    <path
                            d="M4 6h16M4 12h8m-8 6h16"
                            stroke-linecap="round"
                            stroke-linejoin="round"
                            stroke-width="2"
                    />
                </svg>
            </div>
            <ul
                    class="menu dropdown-content z-1 mt-3 w-52 menu-sm rounded-box bg-base-100 p-2 shadow"
                    tabindex="-1"
            >
                <li><a href={routerLink("/wifi")}>Wifi</a></li>
            </ul>
        </div>
        <a class="btn btn-ghost text-xl" href={routerLink("/")}>Home</a>
    </div>
    <div class="navbar-center hidden lg:flex">
        <ul class="menu menu-horizontal px-1">
            <li><a href={routerLink("/wifi")}>Wifi</a></li>
        </ul>
    </div>
</div>
