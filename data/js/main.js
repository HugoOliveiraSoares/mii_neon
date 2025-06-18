window.addEventListener("DOMContentLoaded", (event) => {
  adjustTabs();
  fetchEffects();
  _colorPickInit();
  getBrightFromUI();
  setSegmentColor();
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
    // color: "#506478",
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
    if (!selectedClass) return; //TODO: a cor deve ser trocada mesmo que não tenha um path selecionado

    const paths = document.getElementsByClassName(selectedClass);
    const hex = color.hexString;

    for (const path of paths) {
      path.style.fill = hex;
    }
    // setSegmentColor(color.hexString);
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

function getClassPaths() {
  const miiSvg = document.getElementById("mii-neon");
  const paths = miiSvg.querySelectorAll("svg path");
  const clazzSet = new Set();

  paths.forEach((path) => {
    path.classList.forEach((classe) => clazzSet.add(classe));
  });

  return Array.from(clazzSet);
}

// function setSegmentColor(cor) {
//   const clazz = getClassPaths();
//   let lastSelected = null;
//   let lastStroke = "";
//   let lastStrokeWidth = "";
//
//   clazz.forEach((_clazz) => {
//     const paths = document.getElementsByClassName(_clazz);
//
//     for (let index = 0; index < paths.length; index++) {
//       const path = paths[index];
//       path.addEventListener("click", function () {
//         if (lastSelected !== path) {
//           lastSelected.style.stroke = lastStroke;
//           lastSelected.style.strokeWidth = lastStrokeWidth;
//         }
//
//         lastStroke = this.getAttribute("stroke");
//         lastStrokeWidth = this.getAttribute("stroke-width");
//
//         path.style.fill = cor;
//         path.style.stroke = "white";
//         path.style.strokeWidth = "1";
//
//         lastSelected = path;
//       });
//     }
//   });
// }

function setSegmentColor() {
  const clazzes = getClassPaths();

  clazzes.forEach((_clazz) => {
    const paths = document.getElementsByClassName(_clazz);

    // Adiciona o ouvinte de clique apenas no primeiro elemento do grupo
    if (paths.length > 0) {
      paths[0].addEventListener("click", function () {
        // Restaurar estilos anteriores
        lastSelectedPaths.forEach((path) => {
          path.style.stroke = lastStrokeMap.get(path) || "";
          path.style.strokeWidth = lastStrokeWidthMap.get(path) || "";
        });

        // Limpar dados anteriores
        lastSelectedPaths = [];
        lastStrokeMap.clear();
        lastStrokeWidthMap.clear();

        selectedClass = _clazz;

        // Aplicar novo estilo aos paths com a mesma classe
        for (const path of paths) {
          lastStrokeMap.set(path, path.getAttribute("stroke"));
          lastStrokeWidthMap.set(path, path.getAttribute("stroke-width"));

          path.style.stroke = "white";
          path.style.strokeWidth = "1";
          lastSelectedPaths.push(path);
        }
      });
    }
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
  msg.textContent = "Conectando...";
  const form = new FormData();
  form.append("ssid", ssid);
  form.append("pass", pass);
  try {
    const res = await fetch("/savewifi", { method: "POST", body: form });
    if (res.ok) {
      msg.textContent = "Salvo! Reiniciando...";
    } else {
      msg.textContent = "Erro ao salvar Wi-Fi";
    }
  } catch (e) {
    msg.textContent = "Erro de comunicação";
  }
}
