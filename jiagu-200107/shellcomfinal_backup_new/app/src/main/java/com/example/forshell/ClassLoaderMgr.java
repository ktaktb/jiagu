package com.example.forshell;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;

import org.apache.http.util.EncodingUtils;

//import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;
import android.widget.EditText;
import android.widget.Toast;

import dalvik.system.DexClassLoader;

public class ClassLoaderMgr {

	static ClassLoaderMgr myInstance = null;
	static ClassLoader localDexClassLoader = null;
	public boolean flag = false;
	AssetManager assetManager;

	private ClassLoaderMgr() {

	}

	public static ClassLoaderMgr getInstance() {
		if (null == myInstance) {

			myInstance = new ClassLoaderMgr();
		}

		return myInstance;
	}

	public ClassLoader setClassLoader(AssetManager assetManager, Context ctx)
			throws IOException {
		ClassLoader myLoader = this.getClass().getClassLoader();
		ClassLoader parent = myLoader.getParent();

		System.out.println(parent.getClass().getName());

		InputStream is = null;
		FileOutputStream fos = null;
		File filefir = ctx.getFilesDir();
		String datapath = filefir.getAbsolutePath();
		System.out.println(datapath);
		datapath = datapath.substring(0, datapath.length() - 6);
//		String sourcefilename = filefir.getAbsolutePath() + "/test1.apk";
		//String fileName = filefir.getAbsolutePath() + "/test1.apk";
//		System.out.println(datapath);
		if (!flag) {
//
//			File file1 = new File(fileName);
//			if (!file1.exists()) {
//				System.out.println("copy files");
//				try {
//					is = ctx.getAssets().open("test1.apk");
//					try {
//						int count = is.available();
//						byte[] b = new byte[count];
//						is.read(b);
//						fos = ctx.openFileOutput("test1.apk",
//								Context.MODE_PRIVATE);
//						fos.write(b);
//						fos.close();
//						is.close();
//
//					} catch (FileNotFoundException e) {
//						e.printStackTrace();
//					}
//
//				} catch (IOException e1) {
//
//					e1.printStackTrace();
//				}
//			}

		    String fileName = filefir.getAbsolutePath() + "/class.bin";
			File file2 = new File(fileName);
			if (file2.exists()) {
				// System.out.println("file exist! delete file");
				System.out.println("upgrate bin file");
				//file2.delete();
			}
			if (!file2.exists()) {
			System.out.println("copy bin files");
			try {
				is = ctx.getAssets().open("class.bin");
				try {
					int count = is.available();
					byte[] b = new byte[count];
					is.read(b);
					fos = ctx.openFileOutput("class.bin", Context.MODE_PRIVATE);
					fos.write(b);
					fos.close();
					is.close();
				} catch (FileNotFoundException e) {
					System.out.println("copy files error");
					e.printStackTrace();
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				System.out.println("open files error");
				e1.printStackTrace();
			}
			}
			fileName = filefir.getAbsolutePath() + "/classes.dex";
			File file5 = new File(fileName);			
			if (file5.exists())
			{
				//System.out.println("file exist! delete file");
				System.out.println("upgrate dex file");
				//file5.delete();
			}
			if (!file5.exists()){
			System.out.println("copy dex files");
			try {
				is = ctx.getAssets().open("classes.dex");
				try {
					int count = is.available();
					byte [] b = new byte [count];
					is.read(b);
					fos = ctx.openFileOutput("haha.dex",
                            Context.MODE_PRIVATE);
					fos.write(b);
					fos.close();
					is.close();
				} catch (FileNotFoundException e) {
					System.out.println("copy files error");
					e.printStackTrace();
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				System.out.println("open files error");
				e1.printStackTrace();
			}	
			}
			/*
			fileName = filefir.getAbsolutePath() + "/codedata.txt";
			File file6 = new File(fileName);			
			if (file6.exists())
			{
				//System.out.println("file exist! delete file");
				System.out.println("upgrate codedata.txt file");
				//file6.delete();
			}
			if (!file6.exists()){
			System.out.println("copy codedata files");
			try {
				is = ctx.getAssets().open("codedata.txt");
				try {
					int count = is.available();
					byte [] b = new byte [count];
					is.read(b);
					fos = ctx.openFileOutput("codedata.txt",
                            Context.MODE_PRIVATE);
					fos.write(b);
					fos.close();
					is.close();
				} catch (FileNotFoundException e) {
					System.out.println("copy files error");
					e.printStackTrace();
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				System.out.println("open files error");
				e1.printStackTrace();
			}	
			}
			
			*/

			fileName = filefir.getAbsolutePath() + "/config.bin";
			File file99 = new File(fileName);
			if (file99.exists())
			{
				//System.out.println("file exist! delete file");
				System.out.println("upgrate config.bin file");
				//file7.delete();
			}
			if (!file99.exists()){
				System.out.println("copy config.bin files");
				try {
					is = ctx.getAssets().open("config.bin");
					try {
						int count = is.available();
						byte [] b = new byte [count];
						is.read(b);
						fos = ctx.openFileOutput("config.bin",
								Context.MODE_PRIVATE);
						fos.write(b);
						fos.close();
						is.close();
					} catch (FileNotFoundException e) {
						System.out.println("copy files error");
						e.printStackTrace();
					}
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					System.out.println("open files error");
					e1.printStackTrace();
				}
			}
			
			fileName = filefir.getAbsolutePath() + "/codedata2.txt";
			File file11= new File(fileName);			
			if (file11.exists())
			{
				//System.out.println("file exist! delete file");
				System.out.println("upgrate codedata.txt file");
				//file6.delete();
			}
			if (!file11.exists()){
			System.out.println("copy codedata files");
			try {
				is = ctx.getAssets().open("codedata2.txt");
				try {
					int count = is.available();
					byte [] b = new byte [count];
					is.read(b);
					fos = ctx.openFileOutput("codedata.txt",
                            Context.MODE_PRIVATE);
					fos.write(b);
					fos.close();
					is.close();
				} catch (FileNotFoundException e) {
					System.out.println("copy files error");
					e.printStackTrace();
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				System.out.println("open files error");
				e1.printStackTrace();
			}	
			}
			
			
			
			/*
			fileName = filefir.getAbsolutePath() + "/codeinfo.txt";
			File file7 = new File(fileName);			
			if (file7.exists())
			{
				//System.out.println("file exist! delete file");
				System.out.println("upgrate codeinfo.txt file");
				//file7.delete();
			}
			if (!file7.exists()){
			System.out.println("copy codeinfo.txt files");
			try {
				is = ctx.getAssets().open("codeinfo.txt");
				try {
					int count = is.available();
					byte [] b = new byte [count];
					is.read(b);
					fos = ctx.openFileOutput("codeinfo.txt",
                            Context.MODE_PRIVATE);
					fos.write(b);
					fos.close();
					is.close();
				} catch (FileNotFoundException e) {
					System.out.println("copy files error");
					e.printStackTrace();
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				System.out.println("open files error");
				e1.printStackTrace();
			}	 
			}
			
			*/

			fileName = filefir.getAbsolutePath() + "/codeinfo2.txt";
			File file12 = new File(fileName);			
			if (file12.exists())
			{
				//System.out.println("file exist! delete file");
				System.out.println("upgrate codeinfo.txt file");
				//file7.delete();
			}
			if (!file12.exists()){
			System.out.println("copy codeinfo.txt files");
			try {
				is = ctx.getAssets().open("codeinfo2.txt");
				try {
					int count = is.available();
					byte [] b = new byte [count];
					is.read(b);
					fos = ctx.openFileOutput("codeinfo.txt",
                            Context.MODE_PRIVATE);
					fos.write(b);
					fos.close();
					is.close();
				} catch (FileNotFoundException e) {
					System.out.println("copy files error");
					e.printStackTrace();
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				System.out.println("open files error");
				e1.printStackTrace();
			}	 
			}
			
			
			
			fileName = filefir.getAbsolutePath() + "/inv_M.m";
			File file8= new File(fileName);
			if (!file8.exists())
			{
				try {
					is = ctx.getAssets().open("inv_M.m");
					try {
						int count = is.available();
						byte[] b = new byte[count];
						is.read(b);
						fos = ctx.openFileOutput("inv_M.m",
								Context.MODE_PRIVATE);
						fos.write(b);
						fos.close();
						is.close();
					} catch (Exception e) {
						e.printStackTrace();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			fileName = filefir.getAbsolutePath() + "/inv_tables.t";
			File file9 = new File(fileName);
			if (!file9.exists())
			{
				try {
					is = ctx.getAssets().open("inv_tables.t");
					try {
						int count = is.available();
						byte[] b = new byte[count];
						is.read(b);
						fos = ctx.openFileOutput("inv_tables.t",
								Context.MODE_PRIVATE);
						fos.write(b);
						fos.close();
						is.close();
					} catch (Exception e) {
						e.printStackTrace();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			fileName = filefir.getAbsolutePath() + "/soname.txt";
			File file18= new File(fileName);
			if (!file18.exists())
			{
				try {
					is = ctx.getAssets().open("soname.txt");
					try {
						int count = is.available();
						byte[] b = new byte[count];
						is.read(b);
						fos = ctx.openFileOutput("soname.txt",
								Context.MODE_PRIVATE);
						fos.write(b);
						fos.close();
						is.close();
					} catch (Exception e) {
						e.printStackTrace();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			fileName = filefir.getAbsolutePath() + "/lib.bin";
			File file19= new File(fileName);
			if (!file19.exists())
			{
				try {
					is = ctx.getAssets().open("lib.bin");
					try {
						int count = is.available();
						byte[] b = new byte[count];
						is.read(b);
						fos = ctx.openFileOutput("lib.bin",
								Context.MODE_PRIVATE);
						fos.write(b);
						fos.close();
						is.close();
					} catch (Exception e) {
						e.printStackTrace();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			// System.out.println("copy key files");
			// try {
			// is = ctx.getAssets().open("key");
			// try {
			// int count = is.available();
			// byte [] b = new byte [count];
			// is.read(b);
			// File file3 = new File(Environment.getExternalStorageDirectory(),
			// "/key");
			// Log.i("TAG","file new path ="+file3.getAbsolutePath());
			// fos = new FileOutputStream(file3);
			// fos.write(b);
			// fos.close();
			// is.close();
			// } catch (FileNotFoundException e) {
			// e.printStackTrace();
			// }
			//
			// } catch (IOException e1) {
			// // TODO Auto-generated catch block
			// e1.printStackTrace();
			// }

			try {
				PackageManager manager = ctx.getPackageManager();
				PackageInfo info = manager.getPackageInfo(ctx.getPackageName(),
						0);
				String version = info.versionName;
				byte bt[] = new byte[100];
				bt = version.getBytes();
			//	Log.i("ver", "new version is " + version);

				fileName = filefir.getAbsolutePath() + "/versionFile";
				File file3 = new File(fileName);
				if (file3.exists()) {
				//	Log.i("ver", "version file exist");
					String ver = "";
					try {
						FileInputStream out = new FileInputStream(
								file3.getAbsolutePath());
						int lenght = out.available();
						byte[] buffer = new byte[lenght];
						out.read(buffer, 0, lenght);
						out.close();
						ver = EncodingUtils.getString(buffer, "UTF-8");
						//Log.i("ver", "old version is " + ver);
						if (ver.intern() != version.intern()) {
						//	Log.i("ver", "upgrade");

							file3.delete();
							FileOutputStream in = new FileOutputStream(
									file3.getAbsolutePath(), true);
							try {
								in.write(bt, 0, bt.length);
								in.close();
							} catch (IOException e) {
								e.printStackTrace();
							}

							fileName = filefir.getAbsolutePath()
									+ "/dexflag.flag";
							File file4 = new File(fileName);
							if (file4.exists()) {
								System.out.println("upgrate fake dex file");
								file4.delete();
							}
						}
					} catch (Exception e) {
						e.printStackTrace();
					}
				} else {
				//	Log.i("ver", "version file not exist");
					FileOutputStream in = new FileOutputStream(
							file3.getAbsolutePath(), true);
					try {
						in.write(bt, 0, bt.length);
						in.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
				System.out.println("get version error");
			}

			System.loadLibrary("sec");

		}

		return localDexClassLoader;
	}

}
