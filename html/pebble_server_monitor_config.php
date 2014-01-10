<!DOCTYPE html>
<html>
  <head>
    <title>Configurable</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.css" />
    <script src="http://code.jquery.com/jquery-1.9.1.min.js"></script>
    <script src="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.js"></script>
  </head>
  <body>
    <div data-role="page" id="main">
      <div data-role="header" class="jqm-header">
        <h1>Server Monitor Configuration</h1>
      </div>

      <div data-role="content">

        <div data-role="fieldcontain">
          <label for="ip">Server IP Address:</label>

<?php
          if (!isset($_GET['ip'])) {
            $ip = '192.168.1.1';
          } else {
            $ip = $_GET['ip'];
          }
  
          echo '<textarea cols="40" rows="8" name="ip" id="ip">' . $ip . '</textarea>';
?>
        </div>

        <div data-role="fieldcontain">
          <label for="auto_update">Auto-Refresh:</label>
          <select name="auto_update" id="auto_update" data-role="slider">
<?php
          if (!isset($_GET['auto_update'])) {
            $auto_update = '1';
          } else {
            $auto_update = $_GET['auto_update'];
          }
  
          if ($auto_update == '0') {
            $s0 = ' selected';
            $s1 = '';
          } else {
            $s0 = '';
            $s1 = ' selected';
          }

          echo '<option value="0"' . $s0 . '>Off</option>';
          echo '<option value="1"' . $s1 . '>On</option>';
?>
          </select>
        </div>

        <div data-role="fieldcontain">
          <label for="ip">Update interval [seconds]:</label>
<?php
          if (!isset($_GET['update_interval'])) {
            $update_interval = '1';
          } else {
            $update_interval = $_GET['update_interval'];
          }
  
          echo '<textarea cols="40" rows="8" name="update_interval" id="update_interval">' . $update_interval . '</textarea>';
?>
        </div>

        <div class="ui-body ui-body-b">
          <fieldset class="ui-grid-a">
              <div class="ui-block-a"><button type="submit" data-theme="d" id="b-cancel">Cancel</button></div>
              <div class="ui-block-b"><button type="submit" data-theme="a" id="b-submit">Submit</button></div>
            </fieldset>
          </div>
        </div>
      </div>
    </div>
    <script>
      function saveOptions() {
        var options = {
          'ip': $("#ip").val(),
          'auto_update': $("#auto_update").val(),
          'update_interval': $("#update_interval").val(),
        }
        return options;
      }

      $().ready(function() {
        $("#b-cancel").click(function() {
          console.log("Cancel");
          document.location = "pebblejs://close";
        });

        $("#b-submit").click(function() {
          console.log("Submit");

          var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
          console.log("Warping to: " + location);
          console.log(location);
          document.location = location;
        });

      });
    </script>
  </body>
</html>
