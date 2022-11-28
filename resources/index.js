window.onload = () => {
  const sendButton = document.querySelector("#send-button");

  sendButton.addEventListener("click", (ev) => {
    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/send", true);
    xhr.setRequestHeader( 'Content-Type', 'application/x-www-form-urlencoded' );
    xhr.onload = function() {
      if (xhr.status == 200) {
        const data = JSON.parse(decodeURIComponent(xhr.responseText));
        document.querySelector("#target").innerHTML = data["text"];
      } else {
        alert(`Error ${xhr.status}: ${xhr.statusText}`);
      }
    };
    var data = { text: 'こんにちは、世界！' }
    xhr.send(encodeURIComponent(JSON.stringify(data)));
  });
};
