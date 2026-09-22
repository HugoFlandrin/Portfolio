(() => {
  const trigger = document.querySelector(".hero-profile");
  const flipper = document.querySelector(".coin-flipper");
  if (!trigger || !flipper) return;

  let angle = 0;
  let isAnimating = false;
  const isTouchDevice = window.matchMedia("(pointer: coarse)").matches;

  const flip = () => {
    if (isAnimating) return;

    const nextAngle = angle + 180;
    isAnimating = true;

    const animation = flipper.animate(
      [
        { transform: `rotateY(${angle}deg)` },
        { transform: `rotateY(${nextAngle}deg)` },
      ],
      { duration: 900, easing: "ease-in-out", fill: "forwards" }
    );

    animation.onfinish = () => {
      angle = nextAngle;
      flipper.style.transform = `rotateY(${angle}deg)`;
      animation.cancel();
      isAnimating = false;
    };
  };

  if (isTouchDevice) {
    trigger.addEventListener("click", flip);
  } else {
    trigger.addEventListener("mouseenter", flip);
  }
})();
