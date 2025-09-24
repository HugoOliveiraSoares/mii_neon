import { Api } from "./api.js";

// ========== Factory Pattern ==========

// Cria um botão de efeito (li) com classes Bootstrap
export function createEffectButton(effectName) {
  const li = document.createElement("li");
  li.className = "btn btn-secondary w-100 my-2";
  li.textContent = effectName;
  li.dataset.effect = effectName;
  return li;
}

// ========== Renderização de UI ==========

// Renderiza a lista de efeitos no container
export function renderEffects(effects, containerId) {
  const container = document.getElementById(containerId);
  container.innerHTML = "";
  effects.forEach((effect) => {
    container.appendChild(createEffectButton(effect));
  });
}

// Atualiza o valor do brilho no input range
export function setBrightToUi(value) {
  const input = document.getElementById("brightInput");
  if (input) input.value = value;
}

// Exibe uma mensagem no elemento informado
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

// ========== Event Delegation ==========

// Adiciona event delegation para seleção de efeito
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

// Adiciona event delegation para mostrar/ocultar senha
export function setupPasswordToggle(btnId, inputId) {
  const btn = document.getElementById(btnId);
  const input = document.getElementById(inputId);
  if (!btn || !input) return;
  btn.addEventListener("click", () => {
    input.type = input.type === "text" ? "password" : "text";
  });
}

// ========== Modal Bootstrap ==========

// Exibe um modal Bootstrap (requer Bootstrap JS)
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

// ========== Formulários ==========

// Adiciona event listener para formulário de Wi-Fi
export function setupWifiForm(formId, msgContainerId) {
  const form = document.getElementById(formId);
  if (!form) return;
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const ssid = form.querySelector("#ssid").value;
    const pass = form.querySelector("#pass").value;
    if (!ssid || !pass) {
      showMessage("SSID e senha são obrigatórios.", msgContainerId);
      return;
    }
    showMessage("Conectando...", msgContainerId);
    const formData = new FormData(form);
    try {
      const data = await Api.saveWifi(formData);
      if (data.status === "error") {
        showModal("wifiFailModal", data.msg);
        return;
      }
      // Polling para status (simplificado)
      let tentativas = 0;
      const maxTentativas = 20;
      const intervalo = 10000;
      const poll = setInterval(async () => {
        tentativas++;
        const statusData = await Api.fetchWifiStatus();
        if (statusData.status === "success") {
          showMessage("Conectado! Redirecionando...", msgContainerId);
          clearInterval(poll);
          setTimeout(() => (window.location.href = "/"), 2000);
        } else if (
          statusData.status === "fail" ||
          tentativas >= maxTentativas
        ) {
          showModal(
            "wifiFailModal",
            "Falha ao conectar. Verifique SSID/senha.",
          );
          clearInterval(poll);
        }
      }, intervalo);
    } catch (e) {
      showModal("wifiFailModal", "Erro de comunicação");
      showMessage("Erro de comunicação", msgContainerId);
    }
  });
}

// ========== Utilitários ==========

// Preenche o select de redes Wi-Fi
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

  colorPicker.on("color:change", function (color) {
    const _color = {
      rgb: {
        r: color.red,
        g: color.green,
        b: color.blue,
      },
    };

    Api.setColor(_color); //Send color to server
  });
}
