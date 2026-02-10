import {
  adjustTabs,
  setBrightToUi,
  setupColorPicker,
  setupEffectsList,
  initializeCurrentEffect,
} from "./ui.js";

import { Api } from "./api.js";

document.addEventListener("DOMContentLoaded", () => {
  // Set initial state - mode toggle initially hidden
  const modeToggleContainer = document.querySelector('.form-check.form-switch');
  if (modeToggleContainer) {
    modeToggleContainer.style.display = 'none';
    modeToggleContainer.style.opacity = '0';
  }
  
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

      // Carrega brilho
      const brightData = await Api.fetchBright();
      if (brightData.bright !== undefined) {
        setBrightToUi(brightData.bright);
      }
    } catch (e) {
      console.log("Erro ao carregar dados: ", e);
    }
  })();

  // Initialize current effect state
  initializeCurrentEffect();
});
