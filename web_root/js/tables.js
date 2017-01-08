$(document).ready(function() {
    $('#dataTables-example').DataTable({
        responsive: true
    });

    GetSecrets();
})

function GetSecrets() {
    $.ajax({
        url: '/SecMng/GetSecrets',
        type: 'GET',
        dataType: 'json',
        success: function(ret) {
            for(var i = 0; i < ret.accounts.length; i++) {
                var row = document.getElementById('tbody-test').insertRow(i);
                var target = row.insertCell(0);
                var username = row.insertCell(1);
                var secret = row.insertCell(2);
                target.innerHTML = ret.accounts[i].target;
                username.innerHTML = ret.accounts[i].username;
                secret.innerHTML = ret.accounts[i].password;
            }
        }
    });
}
