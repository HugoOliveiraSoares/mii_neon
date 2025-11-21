import { Api } from "./api.js";

export function setBrightToUi(value) {
  const input = document.getElementById("brightInput");
  if (input) input.value = value;
}

export function showMessage(msg, containerId) {
  const el = document.getElementById(containerId);
  if (el) el.textContent = msg;
}

export function adjustTabs() {
  const tabs = document.getElementById("bottomNavBar");
  const colors = document.getElementById("colors");
  const effects = document.getElementById("effects");
  if (window.innerHeight >= 800 && window.innerWidth >= 600) {
    tabs.style.display = "none";
    colors.classList.add("show");
    colors.classList.add("active");
    effects.classList.add("show");
    effects.classList.add("active");
  } else {
    tabs.style.display = "flex";
    effects.classList.remove("show");
    effects.classList.remove("active");
  }
}

export async function setupColorPicker() {
  const rgb = await Api.fetchColor();
  if (!rgb) return null;

  const r = Math.max(0, Math.min(255, rgb.r));
  const g = Math.max(0, Math.min(255, rgb.g));
  const b = Math.max(0, Math.min(255, rgb.b));

  const componenteHex = (componente) => {
    const hex = componente.toString(16);
    return hex.length === 1 ? "0" + hex : hex;
  };

  const hexColor = "#" + componenteHex(r) + componenteHex(g) + componenteHex(b);
  initColorPicker(hexColor);
}

export function setupEffectsList(containerId) {
  const container = document.getElementById(containerId);
  if (!container) return;
  container.addEventListener("click", (e) => {
    const li = e.target.closest("li[data-effect]");
    if (li) {
      Api.setEffect({ effect: li.dataset.effect });
      //TODO: Deixar feedback visual mesmo apos recarregar a pagina
      container
        .querySelectorAll("li")
        .forEach((el) => el.classList.remove("active"));
      li.classList.add("active");
    }
  });
}

export function setupPasswordToggle(btnId, inputId) {
  const btn = document.getElementById(btnId);
  const input = document.getElementById(inputId);
  if (!btn || !input) return;
  btn.addEventListener("click", () => {
    const isPassword = input.type === "password";
    input.type = isPassword ? "text" : "password";

    btn.setAttribute("aria-label", isPassword ? "Ocultar senha" : "Mostrar senha");
    const icon = btn.querySelector("svg");
    if (icon) {
      if (isPassword) {
        icon.innerHTML = `<path d="m644-428-58-58q9-47-27-88t-93-32l-58-58q17-8 34.5-12t37.5-4q75 0 127.5 52.5T660-500q0 20-4 37.5T644-428Zm128 126-58-56q38-29 67.5-63.5T832-500q-50-101-143.5-160.5T480-720q-29 0-57 4t-55 12l-62-62q41-17 84-25.5t90-8.5q151 0 269 83.5T920-500q-23 59-60.5 109.5T772-302Zm20 246L624-222q-35 11-70.5 16.5T480-200q-151 0-269-83.5T40-500q21-53 53-98.5t73-81.5L56-792l56-56 736 736-56 56ZM222-624q-29 26-53 57t-41 67q50 101 143.5 160.5T480-280q20 0 39-2.5t39-5.5l-36-38q-11 3-21 4.5t-21 1.5q-75 0-127.5-52.5T300-500q0-11 1.5-21t4.5-21l-84-82Zm319 93Zm-151 75Z"/>`;
      } else {
        icon.innerHTML = `<path d="M480-320q75 0 127.5-52.5T660-500q0-75-52.5-127.5T480-680q-75 0-127.5 52.5T300-500q0 75 52.5 127.5T480-320Zm0-72q-45 0-76.5-31.5T372-500q0-45 31.5-76.5T480-608q45 0 76.5 31.5T588-500q0 45-31.5 76.5T480-392Zm0 192q-146 0-266-81.5T40-500q54-137 174-218.5T480-800q146 0 266 81.5T920-500q-54 137-174 218.5T480-200Zm0-300Zm0 220q113 0 207.5-59.5T832-500q-50-101-144.5-160.5T480-720q-113 0-207.5 59.5T128-500q50 101 144.5 160.5T480-280Z"/>`;
      }
    }
  });
}

export function showModal(modalId, msg = "") {
  const modalEl = document.getElementById(modalId);
  if (!modalEl) return;
  if (msg) {
    const msgEl = modalEl.querySelector("#msg-modal");
    if (msgEl) msgEl.textContent = msg;
  }
  const modal = new bootstrap.Modal(modalEl);
  modal.show();
}

export async function renderWifiNetworks() {
  const select = document.getElementById("ssid");
  select.innerHTML = "<option>Carregando...</option>";

  const data = await Api.scanWifi();
  if (data.networks) {
    select.innerHTML = "";
    data.networks.forEach((ssid) => {
      const opt = document.createElement("option");
      opt.value = ssid;
      opt.textContent = ssid;
      select.appendChild(opt);
    });
    if (data.networks.length === 0) {
      select.innerHTML = "<option>Nenhuma rede encontrada</option>";
    }
  } else if (data.status === "scanning" || data.status === "started") {
    setTimeout(renderWifiNetworks, 1000);
  } else {
    select.innerHTML = "<option>Formato de dados inválido</option>";
  }
}

function initColorPicker(_color) {
  var _width = 0;

  if (window.innerHeight <= 750) {
    _width = 220;
  } else {
    _width = 300;
  }

  const colorPicker = new iro.ColorPicker("#colorPicker", {
    width: _width,
    color: _color,
    borderWidth: 2,
    borderColor: "var(---my-border-color)",
    layout: [
      {
        component: iro.ui.Wheel,
        options: {
          wheelLightness: false,
        },
      },
      {
        component: iro.ui.Slider,
        options: {
          sliderType: "saturation",
          borderColor: "var(---my-border-color)",
        },
      },
      {
        component: iro.ui.Slider,
        options: {
          sliderType: "value",
          borderColor: "var(---my-border-color)",
        },
      },
    ],
  });

  colorPicker.on("color:change", function(color) {
    const _color = {
      rgb: {
        r: color.red,
        g: color.green,
        b: color.blue,
      },
    };

    Api.setColor(_color);
  });
}
