<?php
/*
  Copyright (c) 2008-2009, Neutron Soutmun <neo.neutron@gmail.com>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions 
  are met:

  1. Redistributions of source code must retain the above copyright 
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright 
     notice, this list of conditions and the following disclaimer in the 
     documentation and/or other materials provided with the distribution.
  3. The names of the authors may not be used to endorse or promote products 
     derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
  POSSIBILITY OF SUCH DAMAGE.

  This code cannot simply be copied and put under the GNU Public License or 
    any other GPL-like (LGPL, GPL2) License.
*/

require_once 'config.php';
require_once 'header.php';
require_once 'networkchk.php';

$ip = $_SERVER['REMOTE_ADDR'];
$config = get_config_by_network($ip, $config_list);
$vserver_id = $config["VSERVER_ID"];

$forward_uri  = $config['NAS_LOGIN_PROTO'] . "://" . $config['NAS_LOGIN_HOST'];
$forward_uri .= !empty($config['NAS_LOGIN_PORT']) ? ":" . $config['NAS_LOGIN_PORT'] : "";
$forward_uri .= "/login.php?sss=" . time();
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Cache-Control" content="no-cache, must-revalidate">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="0">
<title>RahuNAS Authentication</title>
<style>
body {
  font-family: sans-serif;
  font-size: 10pt;
}
a {
  text-decoration: none;
}

a:hover {
  font-weight: bolder;
  color: #FF0000;
}

#loadingpic {
  padding-top: 100px;
  padding-bottom: 30px;
}
</style>
</head>
<body>
<center>
<div id="loadingpic"><img src="/loading.gif"></div>
<div><?php echo _("If it is not redirecting within 3 seconds") . "," . _("click"); ?> <a href="<?php echo $forward_uri ?>"><?php echo _("Login Page"); ?></a></div>
</center>

<script language="JavaScript">
function redirecting() 
{
  redirect_uri = "<?php echo $forward_uri ?>&request_url=" + escape(location);
  location.replace(redirect_uri);
}
setTimeout("redirecting();", 1000);
</script>
</body>
</html>
