(() => {
  const modal = document.getElementById("image-modal");
  const modalImg = document.getElementById("modal-img");

  const openModal = (src, alt) => {
    if (!modal || !modalImg) return;
    modalImg.src = src;
    modalImg.alt = alt || "Expanded screenshot";
    modal.classList.add("is-open");
    modal.setAttribute("aria-hidden", "false");
    document.body.style.overflow = "hidden";
  };

  const closeModal = () => {
    if (!modal || !modalImg) return;
    modal.classList.remove("is-open");
    modal.setAttribute("aria-hidden", "true");
    modalImg.src = "";
    document.body.style.overflow = "";
  };

  document.querySelectorAll(".media-item img").forEach((img) => {
    img.addEventListener("click", () => {
      openModal(img.src, img.alt);
    });
  });

  if (modal) {
    modal.addEventListener("click", (e) => {
      if (e.target === modal) closeModal();
    });
  }

  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape") closeModal();
  });

  const videoSlides = Array.from(document.querySelectorAll(".video-slide"));
  const prevVideoBtn = document.querySelector(".prev-video");
  const nextVideoBtn = document.querySelector(".next-video");
  const videoThumbs = Array.from(document.querySelectorAll(".video-thumb"));
  let videoIndex = 0;

  function showVideo(i) {
    if (!videoSlides.length) return;
    videoSlides.forEach((slide) => slide.classList.remove("active"));
    videoSlides[i].classList.add("active");
    videoThumbs.forEach((thumb, idx) => {
      thumb.classList.toggle("active", idx === i);
    });
  }

  function stopAllVideos() {
    document.querySelectorAll("video").forEach((v) => {
      try {
        v.pause();
      } catch {
        /* ignore */
      }
    });
  }

  if (!videoSlides.length) {
    /* no video on this page */
  } else if (videoSlides.length <= 1) {
    if (prevVideoBtn) prevVideoBtn.style.display = "none";
    if (nextVideoBtn) nextVideoBtn.style.display = "none";
  } else if (prevVideoBtn && nextVideoBtn) {
    prevVideoBtn.addEventListener("click", () => {
      stopAllVideos();
      videoIndex = (videoIndex - 1 + videoSlides.length) % videoSlides.length;
      showVideo(videoIndex);
    });

    nextVideoBtn.addEventListener("click", () => {
      stopAllVideos();
      videoIndex = (videoIndex + 1) % videoSlides.length;
      showVideo(videoIndex);
    });
  }

  videoThumbs.forEach((thumb, idx) => {
    thumb.addEventListener("click", () => {
      stopAllVideos();
      videoIndex = idx;
      showVideo(videoIndex);
    });
  });
})();
