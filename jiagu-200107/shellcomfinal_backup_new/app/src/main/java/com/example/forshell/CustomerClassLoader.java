package com.example.forshell;

//import android.R.string;
import android.R.bool;
import android.annotation.SuppressLint;
import dalvik.system.DexFile;
import dalvik.system.PathClassLoader;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.NoSuchElementException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class CustomerClassLoader extends PathClassLoader {
	/**
	 * Creates a {@code DexClassLoader} that finds interpreted and native code.
	 * Interpreted classes are found in a set of DEX files contained in Jar or
	 * APK files.
	 * 
	 * The path lists are separated using the character specified by the
	 * "path.separator" system property, which defaults to ":".
	 * 
	 * @param dexPath
	 *            the list of jar/apk files containing classes and resources
	 * @param dexOutputDir
	 *            directory where optimized DEX files should be written
	 * @param libPath
	 *            the list of directories containing native libraries; may be
	 *            null
	 * @param parent
	 *            the parent class loader
	 */
	/** originally specified path (just used for {@code toString()}) */

	/** structured lists of path elements */

	/**
	 * Constructs an instance.
	 * 
	 * @param dexPath
	 *            the list of jar/apk files containing classes and resources,
	 *            delimited by {@code File.pathSeparator}, which defaults to
	 *            {@code ":"} on Android
	 * @param optimizedDirectory
	 *            directory where optimized dex files should be written; may be
	 *            {@code null}
	 * @param libraryPath
	 *            the list of directories containing native libraries, delimited
	 *            by {@code File.pathSeparator}; may be {@code null}
	 * @param parent
	 *            the parent class loader
	 */
	private static final boolean VERBOSE_DEBUG = false;
	private final String path;
	private final String libPath;

	private boolean initialized;

	private String[] mPaths;
	private File[] mFiles;
	private ZipFile[] mZips;
	private DexFile[] mDexs;
	private String[] mLibPaths;
	private final String mDexOutputPath;

	public void setIs_write_back(int is_write_back) {
		this.is_write_back = is_write_back==1;
		System.out.println("CustomerClassLoader setIs_write_back ok");
		System.out.println("is_write_back:    " + this.is_write_back);
	}

	private boolean is_write_back;

	public CustomerClassLoader(String path, String optimizedDirectory,
			String libPath, ClassLoader parent) {
		super("", libPath, parent);
		if (path == null || optimizedDirectory == null)
			throw new NullPointerException();
		
		this.path = path;
//		this.createConfigPath();
//		System.out.println("path: " + path);
		this.libPath = libPath;
		this.mDexOutputPath = optimizedDirectory;
		try {
			ensureInit();
		} catch (Exception e) {

		}

	}

//	private void createConfigPath(){
//		String flag = "/files/";
//		int end = this.path.lastIndexOf(flag) + flag.length();
//		String configPath = this.path.substring(0, end) + "config.bin";
//		File f = new File(configPath);
//		try{
////			FileInputStream fileInputStream = new FileInputStream(f);
////			byte[] buf = new byte[fileInputStream.available()];
////			fileInputStream.read(buf);
////			for(int i=0; i<buf.length; i++){
////				System.out.println((int)buf[i]);
////			}
//
//
//			BufferedReader reader = new BufferedReader(new FileReader(f));
//			String is_dex;
//			String ta = reader.readLine();
//			String tb = reader.readLine();
////			System.out.println("ta:    " + ta);
////			System.out.println("tb:    " + tb);
//			is_dex = reader.readLine();
//			System.out.println(configPath + ":    " + is_dex);
//			this.is_write_back = Integer.parseInt(is_dex) == 1;
//			System.out.println("writeback:" + this.is_write_back);
//		} catch (FileNotFoundException e) {
//			e.printStackTrace();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
//	}

	private synchronized void ensureInit() {
		if (initialized) {
			return;
		}

		initialized = true;

		mPaths = path.split(":");
		int length = mPaths.length;

		System.out.println("PathClassLoader: " + mPaths);
		mFiles = new File[length];
		mZips = new ZipFile[length];
		mDexs = new DexFile[length];

		/* open all Zip and DEX files up front */
		for (int i = 0; i < length; i++) {
			System.out.println("My path is: " + mPaths[i]);
			File pathFile = new File(mPaths[i]);
			mFiles[i] = pathFile;

			if (pathFile.isFile()) {
				try {
					mZips[i] = new ZipFile(pathFile);
				} catch (IOException ioex) {
					// expecting IOException and ZipException
					// System.out.println("Failed opening '" + archive + "': " +
					// ioex);
					// ioex.printStackTrace();
				}
				/* we need both DEX and Zip, because dex has no resources */
				try {
					if (mDexOutputPath != null) {
						String outputName = generateOutputName(mPaths[i],
								mDexOutputPath);
						System.out.println("-------------------->loadDex");
						System.out.println("Para1:"+mPaths[i]);
						System.out.println("Para2:"+outputName);
						mDexs[i] = DexFile.loadDex(mPaths[i], outputName, 0);
						System.out.println("init ok");
					}
				} catch (IOException ioex) {
					// System.out.println("------------------------->exception");
				}

			}
		}

		/*
		 * Prep for native library loading.
		 */
		String pathList = System.getProperty("java.library.path", ".");
		String pathSep = System.getProperty("path.separator", ":");
		String fileSep = System.getProperty("file.separator", "/");

		if (libPath != null) {
			if (pathList.length() > 0) {
				pathList += pathSep + libPath;
			} else {
				pathList = libPath;
			}
		}

		mLibPaths = pathList.split(pathSep);
		length = mLibPaths.length;

		// Add a '/' to the end so we don't have to do the property lookup
		// and concatenation later.
		for (int i = 0; i < length; i++) {
			if (!mLibPaths[i].endsWith(fileSep))
				mLibPaths[i] += fileSep;
			if (false)
				System.out.println("Native lib path:  " + mLibPaths[i]);
		}
	}

	/**
	 * Convert a source path name and an output directory to an output file
	 * name.
	 */
	private static String generateOutputName(String sourcePathName,
			String outputDir) {

		StringBuilder newStr = new StringBuilder(80);

		/* start with the output directory */
		newStr.append(outputDir);
		if (!outputDir.endsWith("/"))
			newStr.append("/");

		/* get the filename component of the path */
		String sourceFileName;
		int lastSlash = sourcePathName.lastIndexOf("/");
		if (lastSlash < 0)
			sourceFileName = sourcePathName;
		else
			sourceFileName = sourcePathName.substring(lastSlash + 1);

		/*
		 * Replace ".jar", ".zip", whatever with ".dex". We don't want to use
		 * ".odex", because the build system uses that for files that are paired
		 * with resource-only jar files. If the VM can assume that there's no
		 * classes.dex in the matching jar, it doesn't need to open the jar to
		 * check for updated dependencies, providing a slight performance boost
		 * at startup. The use of ".dex" here matches the use on files in
		 * /data/dalvik-cache.
		 */
		int lastDot = sourceFileName.lastIndexOf(".");
		if (lastDot < 0)
			//newStr.append(sourceFileName);
			newStr.append("classes");
		else
			//newStr.append(sourceFileName, 0, lastDot);
		    newStr.append("classes", 0, 7);
		newStr.append(".dex");

		if (VERBOSE_DEBUG)
			System.out.println("Output file will be " + newStr.toString());
		return newStr.toString();
	}
	public native static void writeback(String name);
	/**
	 * Finds a class. This method is called by {@code loadClass()} after the
	 * parent ClassLoader has failed to find a loaded class of the same name.
	 * 
	 * @param name
	 *            The "binary name" of the class to search for, in a
	 *            human-readable form like "java.lang.String" or
	 *            "java.net.URLClassLoader$3$1".
	 * @return the {@link Class} object representing the class
	 * @throws ClassNotFoundException
	 *             if the class cannot be found
	 */
	@Override
	protected Class<?> findClass(String name) throws ClassNotFoundException {
		ensureInit();

		 System.out.println("PathClassLoader " + this + ": findClass '" + name
		 + "'");
        
		byte[] data = null;
		int length = mPaths.length;
		//String wbname = name.replace('$', '.');
		boolean res = false;
		int len = name.length();
		String strtemp;
		String[] str = {"android.support","com.baidu.","com.amap","com.sina","com.google","com.tencent.","com.alipay","com.facebook","com.alibaba.","com.xiaomi","com.taobao", "org.jivesoftware", "com.easemob", "com.umeng", "com.bumptech", "okhttp3"};
		//System.out.println("str length"+str.length());
		if(len >= 7){
			strtemp = "";
		    strtemp = name.substring(0, 7);
		    res = res || str[15].equals(strtemp);
		}
		if(len>=8){
		    strtemp = "";
		    strtemp = name.substring(0, 8);
		    res = res || str[2].equals(strtemp);
		    strtemp = "";
		    strtemp = name.substring(0, 8);
		    res = res || str[3].equals(strtemp);
		}
		if(len >= 9){
			strtemp = "";
		    strtemp = name.substring(0, 9);
		    res = res || str[13].equals(strtemp);
		}
		if(len>=10){
			strtemp = "";
			strtemp = name.substring(0, 10);
			res = res || str[1].equals(strtemp);
			strtemp = "";
			strtemp = name.substring(0, 10);
			res = res || str[4].equals(strtemp);
			strtemp = "";
			strtemp = name.substring(0, 10);
			res = res || str[6].equals(strtemp);
	        strtemp = "";
		    strtemp = name.substring(0, 10);
		    res = res || str[9].equals(strtemp);
		    strtemp = "";
		    strtemp = name.substring(0, 10);
		    res = res || str[10].equals(strtemp);			
		}
		if(len >= 11){
			strtemp = "";
		    strtemp = name.substring(0, 11);
		    res = res || str[12].equals(strtemp);
		}
		if(len>=12){
			strtemp = "";
			strtemp = name.substring(0, 12);
			res = res || str[5].equals(strtemp);

			strtemp = "";
			strtemp = name.substring(0, 12);
			res = res || str[7].equals(strtemp);
			
			strtemp = "";
			strtemp = name.substring(0, 12);
			res = res || str[8].equals(strtemp);	
			
			strtemp = "";
		    strtemp = name.substring(0, 12);
		    res = res || str[14].equals(strtemp);
		}
		if(len>=15){
			strtemp = "";
			strtemp = name.substring(0, 15);
			res = res || str[0].equals(strtemp);	
		}
		if(len >= 16){
			strtemp = "";
		    strtemp = name.substring(0, 16);
		    res = res || str[11].equals(strtemp);
		}

        if(res != true && this.is_write_back)
        {
		    writeback(name);
        }



		for (int i = 0; i < length; i++) {

			if (mDexs[i] != null) {
				String slashName = name.replace('.', '/');
				Class clazz = mDexs[i].loadClass(slashName, this);
				System.out.println("find class ok");				
				if (clazz != null)
					return clazz;
				else {
					System.out.println("find class failed");
					try { 
						if (VERBOSE_DEBUG)
							System.out.println("to find class"
									+ "in super classloader");
						Class cla = super.findClass(name);
						if (cla != null) {
							if (VERBOSE_DEBUG)
								System.out.println("found class"
										+ "in super classloader");
							return cla;
						}
					} catch (ClassNotFoundException e) {

					}

				}
			} else if (mZips[i] != null) {
				String fileName = name.replace('.', '/') + ".class";
				data = loadFromArchive(mZips[i], fileName);
			} else {
				File pathFile = mFiles[i];
				if (pathFile.isDirectory()) {
					String fileName = mPaths[i] + "/" + name.replace('.', '/')
							+ ".class";
					data = loadFromDirectory(fileName);
				} else {
					// System.out.println("PathClassLoader: can't find '"
					// + mPaths[i] + "'");
				}

			}

			/*
			 * --this doesn't work in current version of Dalvik-- if (data !=
			 * null) { System.out.println("--- Found class " + name + " in zip["
			 * + i + "] '" + mZips[i].getName() + "'"); int dotIndex =
			 * name.lastIndexOf('.'); if (dotIndex != -1) { String packageName =
			 * name.substring(0, dotIndex); synchronized (this) { Package
			 * packageObj = getPackage(packageName); if (packageObj == null) {
			 * definePackage(packageName, null, null, null, null, null, null,
			 * null); } } }
			 * 
			 * return defineClass(name, data, 0, data.length); }
			 */
		}

		throw new ClassNotFoundException(name + " in loader " + this);
	}

//	/**
//	 * Finds a resource. This method is called by {@code getResource()} after
//	 * the parent ClassLoader has failed to find a loaded resource of the same
//	 * name.
//	 *
//	 * @param name
//	 *            The name of the resource to find
//	 * @return the location of the resource as a URL, or {@code null} if the
//	 *         resource is not found.
//	 */

//	@Override
//	protected URL findResource(String name) {
//		System.out.println("java findResourse " + name);
//		URL url = super.findResource(name);
//		if (url != null) {
//			if (VERBOSE_DEBUG)
//				System.out.println("  found " + name + " in "
//						+ "super classloader");
//			return url;
//		}
//		int length = mPaths.length;
//
//		for (int i = 0; i < length; i++) {
//			File pathFile = mFiles[i];
//			ZipFile zip = mZips[i];
//			if (!mPaths[i].endsWith(".dex")) {
//				if (zip.getEntry(name) != null) {
//					if (VERBOSE_DEBUG)
//						System.out.println("  found " + name + " in "
//								+ pathFile);
//					try {
//						// File.toURL() is compliant with RFC 1738 in always
//						// creating absolute path names. If we construct the
//						// URL by concatenating strings, we might end up with
//						// illegal URLs for relative names.
//						return new URL("jar:" + pathFile.toURL() + "!/" + name);
//					} catch (MalformedURLException e) {
//						throw new RuntimeException(e);
//					}
//				}
//			} else {
//				if (VERBOSE_DEBUG)
//					System.out.println("CustomerClassLoader: can't find '"
//							+ mPaths[i] + "'");
//			}
//		}
//
//		if (VERBOSE_DEBUG)
//			System.out.println("  resource " + name + " not found");
//
//		return null;
//	}

	/*
	 * Load the contents of a file from a file in a directory.
	 * 
	 * Returns null if the class wasn't found.
	 */
	private byte[] loadFromDirectory(String path) {
		RandomAccessFile raf;
		byte[] fileData;

		// System.out.println("Trying to load from " + path);
		try {
			raf = new RandomAccessFile(path, "r");
		} catch (FileNotFoundException fnfe) {
			// System.out.println("  Not found: " + path);
			return null;
		}

		try {
			fileData = new byte[(int) raf.length()];
			raf.read(fileData);
			raf.close();
		} catch (IOException ioe) {
			System.err.println("Error reading from " + path);
			// swallow it, return null instead
			fileData = null;
		}

		return fileData;
	}

	/*
	 * Load a class from a file in an archive. We currently assume that the file
	 * is a Zip archive.
	 * 
	 * Returns null if the class wasn't found.
	 */
	private byte[] loadFromArchive(ZipFile zip, String name) {
		ZipEntry entry;

		entry = zip.getEntry(name);
		if (entry == null)
			return null;

		ByteArrayOutputStream byteStream;
		InputStream stream;
		int count;

		/*
		 * Copy the data out of the stream. Because we got the ZipEntry from a
		 * ZipFile, the uncompressed size is known, and we can set the initial
		 * size of the ByteArrayOutputStream appropriately.
		 */
		try {
			stream = zip.getInputStream(entry);
			byteStream = new ByteArrayOutputStream((int) entry.getSize());
			byte[] buf = new byte[4096];
			while ((count = stream.read(buf)) > 0)
				byteStream.write(buf, 0, count);

			stream.close();
		} catch (IOException ioex) {
			// System.out.println("Failed extracting '" + archive + "': "
			// +ioex);
			return null;
		}

		// System.out.println("  loaded from Zip");
		return byteStream.toByteArray();
	}

	/*
	 * Figure out if "name" is a member of "archive".
	 */
//	private boolean isInArchive(ZipFile zip, String name) {
//		System.out.println("java isInArchive " + name);
//		return zip.getEntry(name) != null;
//	}

	/**
	 * Finds a native library. This method is called after the parent
	 * ClassLoader has failed to find a native library of the same name.
	 * 
	 * @param libname
	 *            The name of the library to find
	 * @return the complete path of the library, or {@code null} if the library
	 *         is not found.
	 */
//	public String findLibrary(String libname) {
//		System.out.println("java findLibrary " + libname);
//		//ensureInit();
//
//		String fileName = System.mapLibraryName(libname);
//		for (int i = 0; i < mLibPaths.length; i++) {
//			String pathName = mLibPaths[i] + fileName;
//			File test = new File(pathName);
//
//			if (test.exists())
//				return pathName;
//			else {
//				String path = super.findLibrary(libname);
//				if (path != null) {
//					if (VERBOSE_DEBUG)
//						System.out.println("  found " + libname
//								+ "in super classLoader");
//					return path;
//				}
//			}
//		}
//
//		return null;
//	}

	/**
	 * Returns package information for the given package. Unfortunately, the
	 * PathClassLoader doesn't really have this information, and as a non-secure
	 * ClassLoader, it isn't even required to, according to the spec. Yet, we
	 * want to provide it, in order to make all those hopeful callers of
	 * <code>myClass.getPackage().getName()</code> happy. Thus we construct a
	 * Package object the first time it is being requested and fill most of the
	 * fields with dummy values. The Package object is then put into the
	 * ClassLoader's Package cache, so we see the same one next time. We don't
	 * create Package objects for null arguments or for the default package.
	 * <p>
	 * There a limited chance that we end up with multiple Package objects
	 * representing the same package: It can happen when when a package is
	 * scattered across different JAR files being loaded by different
	 * ClassLoaders. Rather unlikely, and given that this whole thing is more or
	 * less a workaround, probably not worth the effort.
	 * 
	 * @param name
	 *            the name of the class
	 * @return the package information for the class, or {@code null} if there
	 *         is not package information available for it
	 */
//	@SuppressLint("NewApi")
//	@Override
//	protected Package getPackage(String name) {
//		System.out.println("java getPackage " + name);
//		if (name != null && !"".equals(name)) {
//			synchronized (this) {
//				Package pack = super.getPackage(name);
//
//				if (pack == null) {
//					pack = definePackage(name, "Unknown", "0.0", "Unknown",
//							"Unknown", "0.0", "Unknown", null);
//				}
//
//				return pack;
//			}
//		}
//
//		return null;
//	}

}