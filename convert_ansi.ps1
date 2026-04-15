$path = 'C:\Games\Gothic_Dev\zSimpleAcceleration\src\Plugin.hpp'
# Читаем исходный файл как UTF8 (без BOM), чтобы не потерять кириллицу при чтении
$content = Get-Content -Path $path -Encoding UTF8
# Записываем обратно в 1251 (ANSI)
[System.IO.File]::WriteAllLines($path, $content, [System.Text.Encoding]::GetEncoding(1251))
