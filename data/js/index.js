import {
  adjustTabs,
  renderEffects,
  setBrightToUi,
  setupColorPicker,
  setupEffectsList,
} from "./ui.js";

import { Api } from "./api.js";

document.addEventListener("DOMContentLoaded", () => {
  // Inicializa UI
  adjustTabs();
  setupColorPicker();
  setupEffectsList("effects-container");
  setBrightToUi();

  (async () => {
    try {
      // Eventos
      document.getElementById("brightInput").addEventListener("input", (e) => {
        Api.setBright({ bright: e.target.value });
      });

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
    } catch (e) {
      console.log("Erro ao carregar dados: ", e);
    }
  })();
});
