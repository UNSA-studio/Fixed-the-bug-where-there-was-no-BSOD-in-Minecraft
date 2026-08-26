param(
    [string]$StopCode = "C0000005",
    [int]$WinL = -1,
    [int]$WinT = -1,
    [int]$WinR = -1,
    [int]$WinB = -1
)

# Blue overlay drawn exactly over the (now dead) Minecraft window while the
# JVM writes its own hs_err report after a native crash. Can relaunch the game.
Add-Type -AssemblyName PresentationFramework, PresentationCore, WindowsBase

$blue = [System.Windows.Media.BrushConverter]::new().ConvertFromString("#0078D7")
$white = [System.Windows.Media.Brush]::White

$window = New-Object System.Windows.Window
$window.WindowStyle = 'None'
$window.ResizeMode = 'NoResize'
$window.Topmost = $true
$window.Background = $blue
$window.ShowInTaskbar = $false

if ($WinR -gt $WinL -and $WinB -gt $WinT -and $WinL -ge 0 -and $WinT -ge 0) {
    # Sit exactly where the Minecraft window was.
    $window.WindowStartupLocation = 'Manual'
    $window.Left = $WinL
    $window.Top = $WinT
    $window.Width = $WinR - $WinL
    $window.Height = $WinB - $WinT
} else {
    # Fallback: centered window roughly the size of the game window.
    $window.WindowStartupLocation = 'CenterScreen'
    $window.Width = 854
    $window.Height = 500
}
$window.KeyDown.Add({ if ($_.Key -eq 'Escape') { $window.Close() } })

$panel = New-Object System.Windows.Controls.StackPanel
$panel.Margin = '48,40,48,24'

$face = New-Object System.Windows.Controls.TextBlock
$face.Text = ':('
$face.FontSize = 72
$face.FontFamily = 'Segoe UI'
$face.Foreground = $white
$panel.Children.Add($face) | Out-Null

function Add-Line([string]$text, [double]$size = 16, [double]$topMargin = 10) {
    $tb = New-Object System.Windows.Controls.TextBlock
    $tb.Text = $text
    $tb.FontSize = $size
    $tb.FontFamily = 'Segoe UI'
    $tb.Foreground = $white
    $tb.Margin = "0,$topMargin,0,0"
    $tb.TextWrapping = 'Wrap'
    $panel.Children.Add($tb) | Out-Null
    return $tb
}

Add-Line 'Your PC ran into a problem and needs to restart.' 30 24 | Out-Null
Add-Line "We're just collecting some error info, and then we'll restart for you." 22 8 | Out-Null

$percent = Add-Line '0% complete' 26 18

$status = Add-Line 'Waiting for Minecraft to finish writing its crash log...' 20 10

$codeLine = Add-Line "Stop code: 0x$StopCode" 20 26

$buttonRow = New-Object System.Windows.Controls.StackPanel
$buttonRow.Orientation = 'Horizontal'
$buttonRow.Margin = '0,36,0,0'

function Make-Button([string]$label) {
    $b = New-Object System.Windows.Controls.Button
    $b.Content = $label
    $b.Padding = '16,6'
    $b.Margin = '0,0,12,0'
    $b.Background = [System.Windows.Media.BrushConverter]::new().ConvertFromString('#106EBE')
    $b.Foreground = $white
    $b.BorderThickness = '0'
    return $b
}

$closeBtn = Make-Button 'Close this screen'
$closeBtn.Add_Click({ $window.Close() })
$buttonRow.Children.Add($closeBtn) | Out-Null

$window.Content = $panel
$panel.Children.Add($buttonRow) | Out-Null

# Find the crashing Minecraft process: a java process running BootstrapLauncher.
$mc = Get-CimInstance Win32_Process -Filter "Name LIKE 'java%'" |
    Where-Object { $_.CommandLine -match 'BootstrapLauncher' -and $_.CommandLine -match '--gameDir' } |
    Select-Object -First 1

if ($mc) {
    $status.Text = 'Collecting error info... you can close this screen anytime.'
}

$timer = New-Object System.Windows.Threading.DispatcherTimer
$timer.Interval = [TimeSpan]::FromSeconds(1)
$script:ticks = 0
$timer.Add_Tick({
    $script:ticks++
    $pct = [Math]::Min(99, $script:ticks * 3)
    $percent.Text = "$pct% complete"

    $proc = Get-Process -Id $mc.ProcessId -ErrorAction SilentlyContinue
    if ($null -eq $proc) {
        $timer.Stop()
        $percent.Text = '100% complete'
        $status.Text = 'Minecraft exited. Relaunching...'
        Start-Sleep -Seconds 2
        if ($mc.CommandLine) {
            Start-Process -FilePath "$env:ComSpec" -ArgumentList "/c $($mc.CommandLine)"
        }
        $window.Close()
    }
})
$timer.Start()

[void]$window.ShowDialog()