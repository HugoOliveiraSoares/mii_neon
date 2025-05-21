document.addEventListener("DOMContentLoaded", function () {
  const themeSelector = document.getElementById("bd-theme");
  const htmlElement = document.documentElement;

  // Verifica se há um tema salvo no localStorage
  const savedTheme = localStorage.getItem("selectedTheme");
  if (savedTheme) {
    htmlElement.setAttribute("data-bs-theme", savedTheme);
    setSelectedOption(themeSelector, savedTheme);
  }

  // Evento para atualizar o tema e salvar a escolha do usuário
  themeSelector.addEventListener("change", function () {
    const selectedOption = themeSelector.options[themeSelector.selectedIndex];
    const theme = selectedOption.getAttribute("data-bs-theme-value");

    htmlElement.setAttribute("data-bs-theme", theme);
    localStorage.setItem("selectedTheme", theme);
  });

  // Função para marcar a opção correta no select
  function setSelectedOption(selectElement, theme) {
    for (let option of selectElement.options) {
      if (option.getAttribute("data-bs-theme-value") === theme) {
        option.selected = true;
        break;
      }
    }
  }
});
