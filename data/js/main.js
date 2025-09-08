window.addEventListener("DOMContentLoaded", (event) => {
  adjustTabs();
  fetchEffects();
  _colorPickInit();
  getBrightFromUI();
  fetchBright();
  // Carrega redes ao abrir a aba
  scanWifi();
});

document.getElementById("wifi-tab").addEventListener("shown.bs.tab", scanWifi);

let selectedClass = null;
let lastSelectedPaths = [];
const lastStrokeMap = new Map();
const lastStrokeWidthMap = new Map();

function _colorPickInit() {
  getColor();
}

function colorPickInit(_color) {
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

    setColor(_color);
  });
}

function adjustTabs() {
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

function selectEffect(effectName) {
  console.log("Efeito selecionado:", effectName);
  const _effect = {
    effect: effectName,
  };
  setEffects(_effect);
}

//TODO: brilho atual usado e definido no ui
function getBrightFromUI() {
  document.getElementById("brightInput").addEventListener("input", function () {
    const _bright = {
      bright: this.value,
    };
    setBrightToBackend(_bright);
  });
}

function setColor(color) {
  console.log(color);
  const options = {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(color),
  };

  fetch("/color", options)
    .then((data) => {
      if (!data.ok) {
        throw Error(data.status);
      }
      return data.json();
    })
    .then((response) => {
      console.log(response);
    })
    .catch((e) => {
      console.log(e);
    });
}

function fetchColor() {
  return fetch("/color")
    .then((response) => response.json())
    .then((data) => {
      if (data.rgb && typeof data.rgb.r !== "undefined") {
        return data.rgb;
      } else {
        throw new Error("Formato de dados inválido" + JSON.stringify(data));
      }
    })
    .catch((error) => {
      console.error("Erro ao buscar cor:", error);
      return null;
    });
}

async function getColor() {
  const rgb = await fetchColor();
  if (!rgb) return null;

  const r = Math.max(0, Math.min(255, rgb.r));
  const g = Math.max(0, Math.min(255, rgb.g));
  const b = Math.max(0, Math.min(255, rgb.b));

  const componenteHex = (componente) => {
    const hex = componente.toString(16);
    return hex.length === 1 ? "0" + hex : hex;
  };

  const hexColor = "#" + componenteHex(r) + componenteHex(g) + componenteHex(b);
  colorPickInit(hexColor);
}

function setBrightToBackend(bright) {
  console.log(bright);
  const options = {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(bright),
  };

  fetch("/bright", options)
    .then((data) => {
      if (!data.ok) {
        throw Error(data.status);
      }
      return data.json();
    })
    .then((response) => {
      console.log(response);
    })
    .catch((e) => {
      console.log(e);
    });
}

function fetchBright() {
  fetch("/bright")
    .then((response) => response.json())
    .then((data) => {
      if (data.bright && typeof data.bright !== "undefined") {
        setBrightToUi(data.bright);
      } else {
        setBrightToUi(0);
      }
    })
    .catch((error) => console.error("Erro ao buscar brilho:", error));
}

function setBrightToUi(_bright) {
  document.getElementById("brightInput").value = _bright;
}

function fetchEffects() {
  fetch("/effects")
    .then((response) => response.json())
    .then((data) => {
      if (data.effects && Array.isArray(data.effects)) {
        createEffectButtons(data.effects);
      }
    })
    .catch((error) => console.error("Erro ao buscar efeitos:", error));
}

function createEffectButtons(effects) {
  const container = document.getElementById("effects-container");

  if (!container) {
    console.error("Elemento #effects-container não encontrado.");
    return;
  }

  container.innerHTML = ""; // Limpa antes de adicionar os novos botões

  effects.forEach((effect) => {
    const button = document.createElement("li");
    button.className = "btn btn-secondary w-100 my-2";
    button.textContent = effect;
    button.onclick = () => selectEffect(effect);

    container.appendChild(button);
  });
}

function setEffects(effect) {
  const options = {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(effect),
  };

  fetch("/effects", options)
    .then((data) => {
      if (!data.ok) {
        throw Error(data.status);
      }
      return data.json();
    })
    .then((response) => {
      console.log(response);
    })
    .catch((e) => {
      console.log(e);
    });
}

async function scanWifi() {
  const select = document.getElementById("ssid");
  select.innerHTML = "<option>Carregando...</option>";
  try {
    const res = await fetch("/scan");
    const data = await res.json();
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
      // Scan ainda em andamento, tente novamente em 1 segundo
      setTimeout(scanWifi, 1000);
    } else {
      select.innerHTML = "<option>Formato de dados inválido</option>";
    }
  } catch (e) {
    select.innerHTML = "<option>Erro ao buscar redes</option>";
  }
}

async function saveWifi(e) {
  e.preventDefault();
  const ssid = document.getElementById("ssid").value;
  const pass = document.getElementById("pass").value;
  const msg = document.getElementById("wifi-msg");

  if (!ssid || !pass) {
    msg.textContent = "SSID e senha são obrigatórios.";
    return;
  }

  msg.textContent = "Conectando...";

  const form = new FormData();
  form.append("ssid", ssid);
  form.append("pass", pass);

  try {
    const res = await fetch("/savewifi", { method: "POST", body: form });
    const data = await res.json();

    console.log("Status code: " + data.status);

    if (data.status === "error") {
      const mensagem = "Erro ao salvar Wi-Fi. " + data.msg;
      console.log(mensagem);
      showModal("wifiFailModal", data.msg);
      return;
    }

    // Polling para status
    let tentativas = 0;
    const maxTentativas = 20;
    const intervalo = 10000; // ms

    const poll = setInterval(async () => {
      tentativas++;
      try {
        const statusRes = await fetch("/wifistatus");
        const statusData = await statusRes.json();
        console.log("Status retornado: " + statusData.status);
        console.log("Tentativa: " + tentativas);
        if (statusData.status === "success") {
          msg.textContent = "Conectado! Redirecionando...";
          console.log("Conectado! Redirecionando...");
          clearInterval(poll);
          setTimeout(() => (window.location.href = "/"), 2000);
        } else if (statusData.status === "fail") {
          showModal(
            "wifiFailModal",
            "Falha ao conectar. Verifique SSID/senha.",
          );
          console.log("Falha ao conectar. Verifique SSID/senha.");
          clearInterval(poll);
        } else if (tentativas >= maxTentativas) {
          console.log("Tempo excedido ao conectar.");
          showModal("wifiFailModal", "Tempo excedido ao conectar.");
          clearInterval(poll);
        }
        // Se status for "connecting" ou "idle", continua polling
      } catch (err) {
        if (err instanceof TypeError) {
          tentativas++;
          console.warn("TypeError capturado, tentando novamente...");
          if (tentativas >= maxTentativas) {
            clearInterval(poll);
            showModal("wifiFailModal", "");
          }
          return;
        } else {
          showModal("wifiFailModal", "Erro de comunicação com o dispositivo.");
          clearInterval(poll);
        }
      }
    }, intervalo);
  } catch (e) {
    showModal("wifiFailModal");
    msg.textContent = "Erro de comunicação";
    console.log(e);
  }
}

function togglePassword() {
  const passInput = document.getElementById("pass");

  if (passInput.type === "password") {
    passInput.type = "text";
  } else {
    passInput.type = "password";
  }
}

function showModal(modalId, msg) {
  const modalContainer = document.getElementById("modalContainer");
  const modalContent = modalContainer.querySelector(`#${modalId}`);

  const msgM = document.getElementById("msg-modal");
  msgM.textContent = msg;

  const minhaModal = new bootstrap.Modal(modalContent);
  minhaModal.show();
}
