(() => {
  const trigger = document.querySelector(".hero-profile");
  const flipper = document.querySelector(".coin-flipper");
  if (!trigger || !flipper) return;

  let angle = 0;

  trigger.addEventListener("mouseenter", () => {
    angle += 180;
    flipper.animate(
      [{ transform: `rotateY(${angle - 180}deg)` }, { transform: `rotateY(${angle}deg)` }],
      { duration: 900, easing: "ease-in-out", fill: "forwards" }
    );
  });
})();
