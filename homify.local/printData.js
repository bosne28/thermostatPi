function fetchData(){
	const url = 'data.json?' + new Date().getTime();
	fetch(url)
        .then(function (response) {
            return response.json();
        })
        .then(function (data) {
            appendData(data);
        })
        .catch(function (err) {
            console.log('error: ' + err);
        });
    
       function appendData(data) {
        let mainContainer = document.getElementById("gauge__text");
        let humContainer = document.getElementById("humVal")
	    mainContainer.innerHTML = "";
        humContainer.innerHTML="";
        for (let i = 0; i < data.length; i++) {
            let div = document.createElement("div");
            let divhumval = document.createElement("div")
            mainContainer.appendChild(div);
            humContainer.appendChild(divhumval);
            var x=Math.round((data[0].Temperature-25)/83.34*100)/100;
            div.innerHTML = data[i].Temperature+" °C";
            document.getElementById("gauge__bar").style.transform="rotate("+x+"turn)";
            divhumval.innerHTML=data[0].Humidity;
        }
    }
}    
fetchData();
setInterval(fetchData,3000);