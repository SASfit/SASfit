[Reflection.Assembly]::LoadWithPartialName('System.Drawing');
[Reflection.Assembly]::LoadWithPartialName('System.Windows.Forms');

$filename = $args[0];
$file = get-item($filename);
$img = [System.Drawing.Image]::Fromfile($file);
[System.Windows.Forms.Clipboard]::SetImage($img);