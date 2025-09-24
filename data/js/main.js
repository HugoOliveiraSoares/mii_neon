import {
  adjustTabs,
  renderEffects,
  renderWifiNetworks,
  setBrightToUi,
  setupColorPicker,
  setupEffectsList,
  setupPasswordToggle,
  setupWifiForm,
} from "./ui.js";

import { Api } from "./api.js";

document.addEventListener("DOMContentLoaded", () => {
  setupPasswordToggle("togglePasswordBtn", "ssidPass");
  adjustTabs();

  (async () => {
    try {
      // Inicializa UI
      setupEffectsList("effects-container");
      setupColorPicker();

      // Carrega efeitos
      const effectsData = await Api.fetchEffects();
      if (effectsData.effects) {
        renderEffects(effectsData.effects, "effects-container");
      }

      // Carrega brilho
      const brightData = await Api.fetchBright();
      if (brightData.bright !== undefined) {
        setBrightToUi(brightData.bright);
      }

      // Wi-Fi
      setupWifiForm("wifiForm", "wifi-msg");

      // Eventos
      document.getElementById("brightInput").addEventListener("input", (e) => {
        Api.setBright({ bright: e.target.value });
      });

      document.getElementById("scanWifiBtn").addEventListener("click", () => {
        console.log("scan wifi");
        renderWifiNetworks();
      });
    } catch (e) {
      console.log("Erro ao carregar dados: ", e);
    }
  })();
});
