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
            if(ret.result == 0) {
                alert("请登录!");
                window.location.href='../html/login.html';
            } else if(ret.result == 1){
                for(var i = 0; i < ret.accounts.length; i++) {
                    var row = document.getElementById('tbody-test').insertRow(i);
                    var target = row.insertCell(0);
                    var username = row.insertCell(1);
                    var secret = row.insertCell(2);
                    target.innerHTML = ret.accounts[i].target;
                    username.innerHTML = ret.accounts[i].username;
                    secret.innerHTML = ret.accounts[i].password;
                    operator.innerHTML = "<button type=\"button\" id=\"btn-edit\" class=\"btn btn-success btn-circle\">" + 
                                             "<i class=\"fa fa-edit\"></i>" +
                                         "</button> " +
                                         "<button type=\"button\" id=\"btn-check\" class=\"btn btn-info btn-circle\">" + 
                                             "<i class=\"fa fa-check\"></i>" +
                                         "</button> " +
                                         "<button type=\"button\" id=\"btn-del\" class=\"btn btn-warning btn-circle\">" +
                                             "<i class=\"fa fa-times\"></i>" +
                                         "</button>";
                }
            }
        }
    });
}

$(function() {
    //新增账号
    $('#btn-add-account').click(function() {
        var row = document.getElementById('tbody-test').insertRow(0);
        var target = row.insertCell(0);
        var username = row.insertCell(1);
        var secret = row.insertCell(2);
        var operator = row.insertCell(3);
        target.innerHTML = "<div contenteditable='true'>请输入目标</div>";
        username.innerHTML = "<div contenteditable='true'>请输入用户名</div>";
        secret.innerHTML = "<div contenteditable='true'>请输入密码</div>";
        operator.innerHTML = "<button type=\"button\" id=\"btn-edit\" class=\"btn btn-success btn-circle\">" + 
                                "<i class=\"fa fa-edit\"></i>" +
                             "</button> " +
                             "<button type=\"button\" id=\"btn-check\" class=\"btn btn-info btn-circle\">" + 
                                "<i class=\"fa fa-check\"></i>" +
                             "</button> " +
                             "<button type=\"button\" id=\"btn-del\" class=\"btn btn-warning btn-circle\">" +
                                "<i class=\"fa fa-times\"></i>" +
                             "</button>";
    });
});
