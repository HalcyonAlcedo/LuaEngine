using System.Diagnostics;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Net.Http;
using System.IO.Compression;
using System.Net;
using IniParser;
using IniParser.Model;
namespace diagno
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        // 创建一个 HttpClient 实例，用于发送 HTTP 请求
        private static readonly HttpClient client = new();

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // 获取当前运行目录的路径
            string path = System.AppDomain.CurrentDomain.BaseDirectory;
            // 获取文件的版本信息
            if (File.Exists(path + "MonsterHunterWorld.exe"))
            {
                // 当程序运行时自动获取数据
                GetData();
            }
            else
            {
                textBox.Text = "未检测到《怪物猎人：世界》运行程序，请将该文件拷贝到怪猎根目录运行。";
            }
        }

        private async void GetData()
        {
            // 定义一个动态对象，用于存储数据
            dynamic data = new System.Dynamic.ExpandoObject();
            
            // 如果当前目录存在 logs/LuaEngine.log 文件，则额外获取该文件的最后 30 行数据
            string logFile = "logs/LuaEngine.log";
            if (File.Exists(logFile))
            {
                textBox.AppendText("获取LuaEngine.log数据\n");
                data.Log = GetLastLines(logFile, 30);
            }

            // 如果当前目录存在 config.ini 和 graphics_option.ini 则额外获取这两个文件的全部数据
            string configFile = "config.ini";
            string graphicsFile = "graphics_option.ini";
            if (File.Exists(configFile))
            {
                textBox.AppendText("获取config.ini数据\n");
                data.Config = File.ReadAllText(configFile);
            }
            if (File.Exists(graphicsFile))
            {
                textBox.AppendText("获取graphics_option.ini数据\n");
                data.Graphics = File.ReadAllText(graphicsFile);
            }

            // 获取目录结构
            textBox.AppendText("获取目录结构数据\n");
            data.DirStructure = await GetDirStructureAsync();
            textBox.Text = "";
            // 将数据输出到文本框
            foreach (var item in data)
            {
                // 把内容的名称和值输出到 TextBox
                textBox.AppendText(item.Key + "\n" + item.Value + "\n---------------------------------------------------\n");
            }
            copylog.IsEnabled = true;
            uplog.IsEnabled = true;
            imgui.IsEnabled = true;
            switchDX.IsEnabled = true;
        }

        // 定义一个方法，用于从文件中读取最后 n 行数据
        private string GetLastLines(string fileName, int n)
        {
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
            // 创建一个队列，用于存储最后 n 行数据
            var queue = new Queue<string>(n);
            // 获取 gbk 和 utf-8 的编码
            var gbk = Encoding.GetEncoding("gbk");
            var utf8 = Encoding.UTF8;
            // 使用 FileStream 读取文件
            using (var stream = new FileStream(fileName, FileMode.Open, FileAccess.Read))
            {
                // 获取文件的长度
                var length = stream.Length;
                // 创建一个缓冲区，用于存储文件的字节
                var buffer = new byte[length];
                // 从文件中读取字节
                stream.Read(buffer, 0, (int)length);
                // 将字节从 gbk 转换为 utf-8
                var converted = Encoding.Convert(gbk, utf8, buffer);
                // 使用 StreamReader 读取转换后的字节
                using (var reader = new StreamReader(new MemoryStream(converted), utf8))
                {
                    // 读取一行数据
                    var line = reader.ReadLine();
                    // 循环直到文件结束
                    while (line != null)
                    {
                        // 将数据入队
                        queue.Enqueue(line);
                        // 如果队列的长度超过 n，就出队
                        if (queue.Count > n)
                        {
                            queue.Dequeue();
                        }
                        // 读取下一行数据
                        line = reader.ReadLine();
                    }
                }
            }
            // 将队列转换为字符串并返回
            return string.Join(Environment.NewLine, queue);
        }

        // 定义一个方法，用于将数据发送到 api 接口
        private async Task PostAsync(string url, string log)
        {
            // 创建一个 StringContent 实例
            var content = new StringContent(log, Encoding.UTF8, "text/plain");

            // 发送 post 请求，并获取响应
            var response = await client.PostAsync(url, content);

            // 如果响应成功，显示响应内容
            if (response.IsSuccessStatusCode)
            {
                var result = await response.Content.ReadAsStringAsync();
                MessageBox.Show(result, "Success");
            }
            else // 如果响应失败，显示错误信息
            {
                var error = await response.Content.ReadAsStringAsync();
                MessageBox.Show(error, "Error");
            }
        }
        private async Task<string> GetDirStructureAsync()
        {
            // 获取当前目录
            string currentDir = Directory.GetCurrentDirectory();
            // 创建一个 StringBuilder 对象存储结果
            StringBuilder sb = new StringBuilder();
            // 遍历当前目录及其所有子目录下的所有文件和目录
            foreach (string path in Directory.GetFileSystemEntries(currentDir, "*", SearchOption.AllDirectories))
            {
                // 获取文件或目录的属性
                FileAttributes attr = File.GetAttributes(path);
                // 判断是文件还是目录
                if (attr.HasFlag(FileAttributes.Directory))
                {
                    // 如果是目录，只输出目录名
                    sb.AppendLine($"Directory: {path}");
                    textBox.AppendText($"Directory: {path}\n");
                }
                else
                {
                    // 如果是文件，输出文件名，MD5值，最后修改日期和版本号（如果存在）
                    sb.AppendLine($"File: {path}");
                    sb.AppendLine($"MD5: {await GetMD5Async(path)}"); // 使用异步方法获取 MD5 值
                    sb.AppendLine($"Last modified: {File.GetLastWriteTime(path)}");
                    sb.AppendLine($"Version: {FileVersionInfo.GetVersionInfo(path).FileVersion ?? "N/A"}");
                    sb.AppendLine();

                    textBox.AppendText($"File: {path}\n");
                }
            }
            // 将结果返回
            return sb.ToString();
        }

        // 定义一个异步方法来计算文件的 MD5 值
        private async Task<string> GetMD5Async(string path)
        {
            // 使用 FileInfo 类来获取文件的大小
            FileInfo fi = new FileInfo(path);
            long size = fi.Length;
            // 如果文件大小超过 50 MB，返回-
            if (size > 50 * 1024 * 1024)
            {
                return " - ";
            }
            // 使用 MD5CryptoServiceProvider 类来创建 MD5 对象
            using (var md5 = new MD5CryptoServiceProvider())
            {
                // 使用异步方法打开文件流
                using (var stream = File.OpenRead(path))
                {
                    // 使用异步方法计算文件流的哈希值
                    var hash = await md5.ComputeHashAsync(stream);
                    // 将哈希值转换为十六进制字符串
                    return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
                }
            }
        }

        private void CopyLog(object sender, RoutedEventArgs e)
        {
            // 获取textBox中的文本
            string text = textBox.Text;

            // 如果文本不为空，复制到剪切板
            if (!string.IsNullOrEmpty(text))
            {
                Clipboard.SetText(text);
                MessageBox.Show("已复制到剪切板");
            }
            else
            {
                MessageBox.Show("文本框为空");
            }
        }

        private void PostLog(object sender, RoutedEventArgs e)
        {

            // 创建一个新窗口
            Window window = new Window();
            window.Title = "请输入你的名字";
            window.Width = 250;
            window.Height = 140;

            // 创建一个网格布局
            Grid grid = new Grid();
            grid.Margin = new Thickness(5);

            // 创建一个输入框
            TextBox userName = new TextBox();
            userName.Text = "用户";
            userName.HorizontalAlignment = HorizontalAlignment.Center;
            userName.VerticalAlignment = VerticalAlignment.Center;
            userName.Height = 20;
            userName.Width = 220;

            // 创建一个确认按钮
            Button button = new Button();
            button.Content = "确认";
            button.HorizontalAlignment = HorizontalAlignment.Center;
            button.VerticalAlignment = VerticalAlignment.Bottom;
            button.Width = 50;
            button.Margin = new Thickness(0, 55, 0, 10);

            // 给按钮添加点击事件
            button.Click += async (s, e) =>
            {
                textBox.AppendText($"\nUser: {userName.Text}\n");
                // 关闭窗口
                window.Close();
                // 获取完成数据后，将数据 post 到一个 api 接口
                string apiUrl = "https://luascript.alcedo.top/diagno/";
                await PostAsync(apiUrl, textBox.Text);
            };

            // 把输入框和按钮添加到网格中
            grid.Children.Add(userName);
            grid.Children.Add(button);

            // 把网格添加到窗口中
            window.Content = grid;

            // 显示窗口
            window.Show();
        }

        private void textBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            textBox.ScrollToEnd();
        }

        private void textBox_Loaded(object sender, RoutedEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            textBox.VerticalScrollBarVisibility = ScrollBarVisibility.Auto;
        }

        private void install_Click(object sender, RoutedEventArgs e)
        {
            // 定义压缩文件的URL和本地文件名
            // 压缩文件的下载地址，你可以根据需要修改
            string zipUrl = "https://luascript.alcedo.top/Download/LuaEngine.zip";

            // 压缩文件的保存路径，这里使用当前目录
            string zipPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "luae.zip");

            // 解压后的文件夹路径，这里使用当前目录
            string extractPath = Directory.GetCurrentDirectory();

            // 备份文件夹的名称，这里使用当前日期时间
            string backupFolder = DateTime.Now.ToString("yyyyMMddHHmmss");

            // 备份文件夹的路径，这里使用当前目录下的backup子目录
            string backupPath = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "backup", backupFolder);

            try
            {
                // 下载压缩文件
                using (WebClient client = new WebClient())
                {
                    client.DownloadFile(zipUrl, zipPath);
                }

                // 打开压缩文件
                using (ZipArchive archive = ZipFile.OpenRead(zipPath))
                {
                    // 遍历压缩文件中的每个条目
                    foreach (ZipArchiveEntry entry in archive.Entries)
                    {
                        // 获取条目的完整路径
                        string entryPath = System.IO.Path.GetFullPath(System.IO.Path.Combine(extractPath, entry.FullName));

                        // 检查是否存在同名文件或文件夹
                        if (File.Exists(entryPath) || Directory.Exists(entryPath))
                        {
                            // 获取备份的完整路径
                            string backupEntryPath = System.IO.Path.Combine(backupPath, entry.FullName);

                            // 创建备份的父目录
                            Directory.CreateDirectory(System.IO.Path.GetDirectoryName(backupEntryPath));

                            // 备份同名文件或文件夹
                            if (File.Exists(entryPath))
                            {
                                // 如果备份路径已经存在同名文件，就覆盖它
                                if (File.Exists(backupEntryPath))
                                {
                                    File.Delete(backupEntryPath);
                                }
                                File.Move(entryPath, backupEntryPath);
                            }
                            else
                            {
                                // 如果备份路径已经存在同名文件夹，就不移动它
                                if (!Directory.Exists(backupEntryPath))
                                {
                                    Directory.Move(entryPath, backupEntryPath);
                                }
                            }
                        }

                    }
                    archive.ExtractToDirectory(".");
                }

                // 删除压缩文件
                File.Delete(zipPath);

                //删除过期文件
                if (File.Exists(System.IO.Path.Combine(extractPath, "hid.dll")))
                {
                    // 删除 hid.dll
                    File.Delete(System.IO.Path.Combine(extractPath, "hid.dll"));
                }
                // 检查 dtdata.dll 是否存在
                if (File.Exists(System.IO.Path.Combine(extractPath, "dtdata.dll")))
                {
                    // 删除 dtdata.dll
                    File.Delete(System.IO.Path.Combine(extractPath, "dtdata.dll"));
                }

                // 显示成功提示
                MessageBox.Show($"LuaEngine已安装完成，旧文件备份至{backupPath}");
            }
            catch (Exception ex)
            {
                // 显示异常信息
                //MessageBox.Show(ex.Message);
            }
        }

        private void switchDX_Click(object sender, RoutedEventArgs e)
        {
            // 读取当前目录下的graphics_option.ini文件
            var parser = new FileIniDataParser();
            try
            {
                IniData graphics = parser.ReadFile("graphics_option.ini");
                string graphicsDX = graphics["GraphicsOption"]["DirectX12Enable"] ?? "On";
                if (graphicsDX == "On")
                {
                    textBox.AppendText("\n关闭DX12\n");
                    graphics["GraphicsOption"]["DirectX12Enable"] = "Off";
                }
                else if (graphicsDX == "Off")
                {
                    textBox.AppendText("\n开启DX12\n");
                    graphics["GraphicsOption"]["DirectX12Enable"] = "On";
                }
                parser.WriteFile("graphics_option.ini", graphics);
            }
            catch (Exception ex)
            {
                textBox.AppendText("\n错误的graphics_option.ini文件\n");
            }
            try
            {
                IniData config = parser.ReadFile("config.ini");
                string configDX = config["GRAPHICS"]["EnableDX12"] ?? "ON";
                if (configDX == "ON")
                {
                    config["GraphicsOption"]["DirectX12Enable"] = "OFF";
                }
                else if (configDX == "OFF")
                {
                    config["GraphicsOption"]["DirectX12Enable"] = "ON";
                }
                parser.WriteFile("config.ini", config);
            }
            catch (Exception ex)
            {
                textBox.AppendText("\n错误的config.ini文件\n");
            }
        }

        private void imgui_Click(object sender, RoutedEventArgs e)
        {
            // 读取当前目录下的graphics_option.ini文件
            var parser = new FileIniDataParser();
            IniData data = parser.ReadFile("graphics_option.ini");

            // 获取LuaEngine.ImgUI的值，如果不存在，就默认为On
            string value = data["LuaEngine"]["ImgUI"] ?? "On";

            // 如果值是On，就改为Off，反之亦然
            if (value == "On")
            {
                textBox.AppendText("\n关闭Imgui\n");
                data["LuaEngine"]["ImgUI"] = "Off";
            }
            else if (value == "Off")
            {
                textBox.AppendText("\n开启Imgui\n");
                data["LuaEngine"]["ImgUI"] = "On";
            }

            // 保存修改后的文件
            parser.WriteFile("graphics_option.ini", data);
        }

        private void uninstall_Click(object sender, RoutedEventArgs e)
        {
            string Path = Directory.GetCurrentDirectory();
            if (File.Exists(System.IO.Path.Combine(Path, "nativePC/plugins/LuaEngine.dll")))
            {
                File.Delete(System.IO.Path.Combine(Path, "nativePC/plugins/LuaEngine.dll"));
                MessageBox.Show("LuaEngine已移除");
            }
        }
    }
}