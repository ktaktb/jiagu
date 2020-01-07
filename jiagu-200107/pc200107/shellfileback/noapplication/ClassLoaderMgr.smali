.class public Lcom/example/forshell/ClassLoaderMgr;
.super Ljava/lang/Object;
.source "ClassLoaderMgr.java"


# static fields
.field static localDexClassLoader:Ljava/lang/ClassLoader;

.field static myInstance:Lcom/example/forshell/ClassLoaderMgr;


# instance fields
.field assetManager:Landroid/content/res/AssetManager;

.field public flag:Z


# direct methods
.method static constructor <clinit>()V
    .locals 1

    .prologue
    const/4 v0, 0x0

    .line 27
    sput-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    .line 28
    sput-object v0, Lcom/example/forshell/ClassLoaderMgr;->localDexClassLoader:Ljava/lang/ClassLoader;

    return-void
.end method

.method private constructor <init>()V
    .locals 1

    .prologue
    .line 32
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    .line 29
    const/4 v0, 0x0

    iput-boolean v0, p0, Lcom/example/forshell/ClassLoaderMgr;->flag:Z

    .line 34
    return-void
.end method

.method public static getInstance()Lcom/example/forshell/ClassLoaderMgr;
    .locals 1

    .prologue
    .line 37
    sget-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    if-nez v0, :cond_0

    .line 39
    new-instance v0, Lcom/example/forshell/ClassLoaderMgr;

    invoke-direct {v0}, Lcom/example/forshell/ClassLoaderMgr;-><init>()V

    sput-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    .line 42
    :cond_0
    sget-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    return-object v0
.end method


# virtual methods
.method public setClassLoader(Landroid/content/res/AssetManager;Landroid/content/Context;)Ljava/lang/ClassLoader;
    .locals 35
    .param p1, "assetManager"    # Landroid/content/res/AssetManager;
    .param p2, "ctx"    # Landroid/content/Context;
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .prologue
    .line 47
    invoke-virtual/range {p0 .. p0}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/Class;->getClassLoader()Ljava/lang/ClassLoader;

    move-result-object v28

    .line 48
    .local v28, "myLoader":Ljava/lang/ClassLoader;
    invoke-virtual/range {v28 .. v28}, Ljava/lang/ClassLoader;->getParent()Ljava/lang/ClassLoader;

    move-result-object v30

    .line 50
    .local v30, "parent":Ljava/lang/ClassLoader;
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    invoke-virtual/range {v30 .. v30}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/Class;->getName()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 52
    const/16 v25, 0x0

    .line 53
    .local v25, "is":Ljava/io/InputStream;
    const/16 v22, 0x0

    .line 54
    .local v22, "fos":Ljava/io/FileOutputStream;
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getFilesDir()Ljava/io/File;

    move-result-object v21

    .line 55
    .local v21, "filefir":Ljava/io/File;
    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v7

    .line 56
    .local v7, "datapath":Ljava/lang/String;
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    move-object/from16 v0, v33

    invoke-virtual {v0, v7}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 57
    const/16 v33, 0x0

    invoke-virtual {v7}, Ljava/lang/String;->length()I

    move-result v34

    add-int/lit8 v34, v34, -0x6

    move/from16 v0, v33

    move/from16 v1, v34

    invoke-virtual {v7, v0, v1}, Ljava/lang/String;->substring(II)Ljava/lang/String;

    move-result-object v7

    .line 61
    move-object/from16 v0, p0

    iget-boolean v0, v0, Lcom/example/forshell/ClassLoaderMgr;->flag:Z

    move/from16 v33, v0

    if-nez v33, :cond_d

    .line 88
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/class.bin"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 89
    .local v20, "fileName":Ljava/lang/String;
    new-instance v14, Ljava/io/File;

    move-object/from16 v0, v20

    invoke-direct {v14, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 90
    .local v14, "file2":Ljava/io/File;
    invoke-virtual {v14}, Ljava/io/File;->exists()Z

    move-result v33

    if-eqz v33, :cond_0

    .line 92
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "upgrate bin file"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 95
    :cond_0
    invoke-virtual {v14}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_1

    .line 96
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy bin files"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 98
    :try_start_0
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "class.bin"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_0
    .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_1

    move-result-object v25

    .line 100
    :try_start_1
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 101
    .local v6, "count":I
    new-array v3, v6, [B

    .line 102
    .local v3, "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 103
    const-string v33, "class.bin"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 104
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 105
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 106
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_1
    .catch Ljava/io/FileNotFoundException; {:try_start_1 .. :try_end_1} :catch_0
    .catch Ljava/io/IOException; {:try_start_1 .. :try_end_1} :catch_1

    .line 117
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_1
    :goto_0
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/classes.dex"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 118
    new-instance v17, Ljava/io/File;

    move-object/from16 v0, v17

    move-object/from16 v1, v20

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 119
    .local v17, "file5":Ljava/io/File;
    invoke-virtual/range {v17 .. v17}, Ljava/io/File;->exists()Z

    move-result v33

    if-eqz v33, :cond_2

    .line 122
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "upgrate dex file"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 125
    :cond_2
    invoke-virtual/range {v17 .. v17}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_3

    .line 126
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy dex files"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 128
    :try_start_2
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "classes.dex"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_2
    .catch Ljava/io/IOException; {:try_start_2 .. :try_end_2} :catch_3

    move-result-object v25

    .line 130
    :try_start_3
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 131
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 132
    .restart local v3    # "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 133
    const-string v33, "haha.dex"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 135
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 136
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 137
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_3
    .catch Ljava/io/FileNotFoundException; {:try_start_3 .. :try_end_3} :catch_2
    .catch Ljava/io/IOException; {:try_start_3 .. :try_end_3} :catch_3

    .line 183
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_3
    :goto_1
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/codedata2.txt"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 184
    new-instance v10, Ljava/io/File;

    move-object/from16 v0, v20

    invoke-direct {v10, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 185
    .local v10, "file11":Ljava/io/File;
    invoke-virtual {v10}, Ljava/io/File;->exists()Z

    move-result v33

    if-eqz v33, :cond_4

    .line 188
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "upgrate codedata.txt file"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 191
    :cond_4
    invoke-virtual {v10}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_5

    .line 192
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy codedata files"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 194
    :try_start_4
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "codedata2.txt"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_4
    .catch Ljava/io/IOException; {:try_start_4 .. :try_end_4} :catch_5

    move-result-object v25

    .line 196
    :try_start_5
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 197
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 198
    .restart local v3    # "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 199
    const-string v33, "codedata.txt"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 201
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 202
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 203
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_5
    .catch Ljava/io/FileNotFoundException; {:try_start_5 .. :try_end_5} :catch_4
    .catch Ljava/io/IOException; {:try_start_5 .. :try_end_5} :catch_5

    .line 252
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_5
    :goto_2
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/codeinfo2.txt"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 253
    new-instance v11, Ljava/io/File;

    move-object/from16 v0, v20

    invoke-direct {v11, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 254
    .local v11, "file12":Ljava/io/File;
    invoke-virtual {v11}, Ljava/io/File;->exists()Z

    move-result v33

    if-eqz v33, :cond_6

    .line 257
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "upgrate codeinfo.txt file"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 260
    :cond_6
    invoke-virtual {v11}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_7

    .line 261
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy codeinfo.txt files"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 263
    :try_start_6
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "codeinfo2.txt"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_6
    .catch Ljava/io/IOException; {:try_start_6 .. :try_end_6} :catch_7

    move-result-object v25

    .line 265
    :try_start_7
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 266
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 267
    .restart local v3    # "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 268
    const-string v33, "codeinfo.txt"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 270
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 271
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 272
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_7
    .catch Ljava/io/FileNotFoundException; {:try_start_7 .. :try_end_7} :catch_6
    .catch Ljava/io/IOException; {:try_start_7 .. :try_end_7} :catch_7

    .line 286
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_7
    :goto_3
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/inv_M.m"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 287
    new-instance v18, Ljava/io/File;

    move-object/from16 v0, v18

    move-object/from16 v1, v20

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 288
    .local v18, "file8":Ljava/io/File;
    invoke-virtual/range {v18 .. v18}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_8

    .line 291
    :try_start_8
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "inv_M.m"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_8
    .catch Ljava/lang/Exception; {:try_start_8 .. :try_end_8} :catch_9

    move-result-object v25

    .line 293
    :try_start_9
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 294
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 295
    .restart local v3    # "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 296
    const-string v33, "inv_M.m"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 298
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 299
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 300
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_9
    .catch Ljava/lang/Exception; {:try_start_9 .. :try_end_9} :catch_8

    .line 308
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_8
    :goto_4
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/inv_tables.t"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 309
    new-instance v19, Ljava/io/File;

    invoke-direct/range {v19 .. v20}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 310
    .local v19, "file9":Ljava/io/File;
    invoke-virtual/range {v19 .. v19}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_9

    .line 313
    :try_start_a
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "inv_tables.t"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_a
    .catch Ljava/lang/Exception; {:try_start_a .. :try_end_a} :catch_b

    move-result-object v25

    .line 315
    :try_start_b
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 316
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 317
    .restart local v3    # "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 318
    const-string v33, "inv_tables.t"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 320
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 321
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 322
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_b
    .catch Ljava/lang/Exception; {:try_start_b .. :try_end_b} :catch_a

    .line 330
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_9
    :goto_5
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/soname.txt"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 331
    new-instance v12, Ljava/io/File;

    move-object/from16 v0, v20

    invoke-direct {v12, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 332
    .local v12, "file18":Ljava/io/File;
    invoke-virtual {v12}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_a

    .line 335
    :try_start_c
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "soname.txt"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_c
    .catch Ljava/lang/Exception; {:try_start_c .. :try_end_c} :catch_d

    move-result-object v25

    .line 337
    :try_start_d
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 338
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 339
    .restart local v3    # "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 340
    const-string v33, "soname.txt"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 342
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 343
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 344
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_d
    .catch Ljava/lang/Exception; {:try_start_d .. :try_end_d} :catch_c

    .line 352
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_a
    :goto_6
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/lib.bin"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 353
    new-instance v13, Ljava/io/File;

    move-object/from16 v0, v20

    invoke-direct {v13, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 354
    .local v13, "file19":Ljava/io/File;
    invoke-virtual {v13}, Ljava/io/File;->exists()Z

    move-result v33

    if-nez v33, :cond_b

    .line 357
    :try_start_e
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v33

    const-string v34, "lib.bin"

    invoke-virtual/range {v33 .. v34}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_e
    .catch Ljava/lang/Exception; {:try_start_e .. :try_end_e} :catch_f

    move-result-object v25

    .line 359
    :try_start_f
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 360
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 361
    .restart local v3    # "b":[B
    move-object/from16 v0, v25

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 362
    const-string v33, "lib.bin"

    const/16 v34, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v22

    .line 364
    move-object/from16 v0, v22

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 365
    invoke-virtual/range {v22 .. v22}, Ljava/io/FileOutputStream;->close()V

    .line 366
    invoke-virtual/range {v25 .. v25}, Ljava/io/InputStream;->close()V
    :try_end_f
    .catch Ljava/lang/Exception; {:try_start_f .. :try_end_f} :catch_e

    .line 398
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_b
    :goto_7
    :try_start_10
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getPackageManager()Landroid/content/pm/PackageManager;

    move-result-object v27

    .line 399
    .local v27, "manager":Landroid/content/pm/PackageManager;
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getPackageName()Ljava/lang/String;

    move-result-object v33

    const/16 v34, 0x0

    move-object/from16 v0, v27

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v1, v2}, Landroid/content/pm/PackageManager;->getPackageInfo(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;

    move-result-object v24

    .line 401
    .local v24, "info":Landroid/content/pm/PackageInfo;
    move-object/from16 v0, v24

    iget-object v0, v0, Landroid/content/pm/PackageInfo;->versionName:Ljava/lang/String;

    move-object/from16 v32, v0

    .line 402
    .local v32, "version":Ljava/lang/String;
    const/16 v33, 0x64

    move/from16 v0, v33

    new-array v4, v0, [B

    .line 403
    .local v4, "bt":[B
    invoke-virtual/range {v32 .. v32}, Ljava/lang/String;->getBytes()[B

    move-result-object v4

    .line 406
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/versionFile"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 407
    new-instance v15, Ljava/io/File;

    move-object/from16 v0, v20

    invoke-direct {v15, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 408
    .local v15, "file3":Ljava/io/File;
    invoke-virtual {v15}, Ljava/io/File;->exists()Z

    move-result v33

    if-eqz v33, :cond_e

    .line 410
    const-string v31, ""
    :try_end_10
    .catch Ljava/lang/Exception; {:try_start_10 .. :try_end_10} :catch_12

    .line 412
    .local v31, "ver":Ljava/lang/String;
    :try_start_11
    new-instance v29, Ljava/io/FileInputStream;

    .line 413
    invoke-virtual {v15}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v33

    move-object/from16 v0, v29

    move-object/from16 v1, v33

    invoke-direct {v0, v1}, Ljava/io/FileInputStream;-><init>(Ljava/lang/String;)V

    .line 414
    .local v29, "out":Ljava/io/FileInputStream;
    invoke-virtual/range {v29 .. v29}, Ljava/io/FileInputStream;->available()I

    move-result v26

    .line 415
    .local v26, "lenght":I
    move/from16 v0, v26

    new-array v5, v0, [B

    .line 416
    .local v5, "buffer":[B
    const/16 v33, 0x0

    move-object/from16 v0, v29

    move/from16 v1, v33

    move/from16 v2, v26

    invoke-virtual {v0, v5, v1, v2}, Ljava/io/FileInputStream;->read([BII)I

    .line 417
    invoke-virtual/range {v29 .. v29}, Ljava/io/FileInputStream;->close()V

    .line 418
    const-string v33, "UTF-8"

    move-object/from16 v0, v33

    invoke-static {v5, v0}, Lorg/apache/http/util/EncodingUtils;->getString([BLjava/lang/String;)Ljava/lang/String;

    move-result-object v31

    .line 420
    invoke-virtual/range {v31 .. v31}, Ljava/lang/String;->intern()Ljava/lang/String;

    move-result-object v33

    invoke-virtual/range {v32 .. v32}, Ljava/lang/String;->intern()Ljava/lang/String;

    move-result-object v34

    move-object/from16 v0, v33

    move-object/from16 v1, v34

    if-eq v0, v1, :cond_c

    .line 423
    invoke-virtual {v15}, Ljava/io/File;->delete()Z

    .line 424
    new-instance v23, Ljava/io/FileOutputStream;

    .line 425
    invoke-virtual {v15}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v33

    const/16 v34, 0x1

    move-object/from16 v0, v23

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-direct {v0, v1, v2}, Ljava/io/FileOutputStream;-><init>(Ljava/lang/String;Z)V
    :try_end_11
    .catch Ljava/lang/Exception; {:try_start_11 .. :try_end_11} :catch_11

    .line 427
    .local v23, "in":Ljava/io/FileOutputStream;
    const/16 v33, 0x0

    :try_start_12
    array-length v0, v4

    move/from16 v34, v0

    move-object/from16 v0, v23

    move/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v4, v1, v2}, Ljava/io/FileOutputStream;->write([BII)V

    .line 428
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V
    :try_end_12
    .catch Ljava/io/IOException; {:try_start_12 .. :try_end_12} :catch_10
    .catch Ljava/lang/Exception; {:try_start_12 .. :try_end_12} :catch_11

    .line 433
    :goto_8
    :try_start_13
    new-instance v33, Ljava/lang/StringBuilder;

    invoke-direct/range {v33 .. v33}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v21 .. v21}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    const-string v34, "/dexflag.flag"

    invoke-virtual/range {v33 .. v34}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v33

    invoke-virtual/range {v33 .. v33}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v20

    .line 435
    new-instance v16, Ljava/io/File;

    move-object/from16 v0, v16

    move-object/from16 v1, v20

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 436
    .local v16, "file4":Ljava/io/File;
    invoke-virtual/range {v16 .. v16}, Ljava/io/File;->exists()Z

    move-result v33

    if-eqz v33, :cond_c

    .line 437
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "upgrate fake dex file"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 438
    invoke-virtual/range {v16 .. v16}, Ljava/io/File;->delete()Z
    :try_end_13
    .catch Ljava/lang/Exception; {:try_start_13 .. :try_end_13} :catch_11

    .line 460
    .end local v4    # "bt":[B
    .end local v5    # "buffer":[B
    .end local v15    # "file3":Ljava/io/File;
    .end local v16    # "file4":Ljava/io/File;
    .end local v23    # "in":Ljava/io/FileOutputStream;
    .end local v24    # "info":Landroid/content/pm/PackageInfo;
    .end local v26    # "lenght":I
    .end local v27    # "manager":Landroid/content/pm/PackageManager;
    .end local v29    # "out":Ljava/io/FileInputStream;
    .end local v31    # "ver":Ljava/lang/String;
    .end local v32    # "version":Ljava/lang/String;
    :cond_c
    :goto_9
    const-string v33, "sec"

    invoke-static/range {v33 .. v33}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

    .line 464
    .end local v10    # "file11":Ljava/io/File;
    .end local v11    # "file12":Ljava/io/File;
    .end local v12    # "file18":Ljava/io/File;
    .end local v13    # "file19":Ljava/io/File;
    .end local v14    # "file2":Ljava/io/File;
    .end local v17    # "file5":Ljava/io/File;
    .end local v18    # "file8":Ljava/io/File;
    .end local v19    # "file9":Ljava/io/File;
    .end local v20    # "fileName":Ljava/lang/String;
    :cond_d
    sget-object v33, Lcom/example/forshell/ClassLoaderMgr;->localDexClassLoader:Ljava/lang/ClassLoader;

    return-object v33

    .line 107
    .restart local v14    # "file2":Ljava/io/File;
    .restart local v20    # "fileName":Ljava/lang/String;
    :catch_0
    move-exception v8

    .line 108
    .local v8, "e":Ljava/io/FileNotFoundException;
    :try_start_14
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 109
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_14
    .catch Ljava/io/IOException; {:try_start_14 .. :try_end_14} :catch_1

    goto/16 :goto_0

    .line 111
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_1
    move-exception v9

    .line 113
    .local v9, "e1":Ljava/io/IOException;
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "open files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 114
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_0

    .line 138
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v17    # "file5":Ljava/io/File;
    :catch_2
    move-exception v8

    .line 139
    .restart local v8    # "e":Ljava/io/FileNotFoundException;
    :try_start_15
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 140
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_15
    .catch Ljava/io/IOException; {:try_start_15 .. :try_end_15} :catch_3

    goto/16 :goto_1

    .line 142
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_3
    move-exception v9

    .line 144
    .restart local v9    # "e1":Ljava/io/IOException;
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "open files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 145
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_1

    .line 204
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v10    # "file11":Ljava/io/File;
    :catch_4
    move-exception v8

    .line 205
    .restart local v8    # "e":Ljava/io/FileNotFoundException;
    :try_start_16
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 206
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_16
    .catch Ljava/io/IOException; {:try_start_16 .. :try_end_16} :catch_5

    goto/16 :goto_2

    .line 208
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_5
    move-exception v9

    .line 210
    .restart local v9    # "e1":Ljava/io/IOException;
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "open files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 211
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_2

    .line 273
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v11    # "file12":Ljava/io/File;
    :catch_6
    move-exception v8

    .line 274
    .restart local v8    # "e":Ljava/io/FileNotFoundException;
    :try_start_17
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "copy files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 275
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_17
    .catch Ljava/io/IOException; {:try_start_17 .. :try_end_17} :catch_7

    goto/16 :goto_3

    .line 277
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_7
    move-exception v9

    .line 279
    .restart local v9    # "e1":Ljava/io/IOException;
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "open files error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 280
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_3

    .line 301
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v18    # "file8":Ljava/io/File;
    :catch_8
    move-exception v8

    .line 302
    .local v8, "e":Ljava/lang/Exception;
    :try_start_18
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_18
    .catch Ljava/lang/Exception; {:try_start_18 .. :try_end_18} :catch_9

    goto/16 :goto_4

    .line 304
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_9
    move-exception v8

    .line 305
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_4

    .line 323
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v19    # "file9":Ljava/io/File;
    :catch_a
    move-exception v8

    .line 324
    .restart local v8    # "e":Ljava/lang/Exception;
    :try_start_19
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_19
    .catch Ljava/lang/Exception; {:try_start_19 .. :try_end_19} :catch_b

    goto/16 :goto_5

    .line 326
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_b
    move-exception v8

    .line 327
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_5

    .line 345
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v12    # "file18":Ljava/io/File;
    :catch_c
    move-exception v8

    .line 346
    .restart local v8    # "e":Ljava/lang/Exception;
    :try_start_1a
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_1a
    .catch Ljava/lang/Exception; {:try_start_1a .. :try_end_1a} :catch_d

    goto/16 :goto_6

    .line 348
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_d
    move-exception v8

    .line 349
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_6

    .line 367
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v13    # "file19":Ljava/io/File;
    :catch_e
    move-exception v8

    .line 368
    .restart local v8    # "e":Ljava/lang/Exception;
    :try_start_1b
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_1b
    .catch Ljava/lang/Exception; {:try_start_1b .. :try_end_1b} :catch_f

    goto/16 :goto_7

    .line 370
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_f
    move-exception v8

    .line 371
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_7

    .line 429
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v4    # "bt":[B
    .restart local v5    # "buffer":[B
    .restart local v15    # "file3":Ljava/io/File;
    .restart local v23    # "in":Ljava/io/FileOutputStream;
    .restart local v24    # "info":Landroid/content/pm/PackageInfo;
    .restart local v26    # "lenght":I
    .restart local v27    # "manager":Landroid/content/pm/PackageManager;
    .restart local v29    # "out":Ljava/io/FileInputStream;
    .restart local v31    # "ver":Ljava/lang/String;
    .restart local v32    # "version":Ljava/lang/String;
    :catch_10
    move-exception v8

    .line 430
    .local v8, "e":Ljava/io/IOException;
    :try_start_1c
    invoke-virtual {v8}, Ljava/io/IOException;->printStackTrace()V
    :try_end_1c
    .catch Ljava/lang/Exception; {:try_start_1c .. :try_end_1c} :catch_11

    goto/16 :goto_8

    .line 441
    .end local v5    # "buffer":[B
    .end local v8    # "e":Ljava/io/IOException;
    .end local v23    # "in":Ljava/io/FileOutputStream;
    .end local v26    # "lenght":I
    .end local v29    # "out":Ljava/io/FileInputStream;
    :catch_11
    move-exception v8

    .line 442
    .local v8, "e":Ljava/lang/Exception;
    :try_start_1d
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_1d
    .catch Ljava/lang/Exception; {:try_start_1d .. :try_end_1d} :catch_12

    goto/16 :goto_9

    .line 455
    .end local v4    # "bt":[B
    .end local v8    # "e":Ljava/lang/Exception;
    .end local v15    # "file3":Ljava/io/File;
    .end local v24    # "info":Landroid/content/pm/PackageInfo;
    .end local v27    # "manager":Landroid/content/pm/PackageManager;
    .end local v31    # "ver":Ljava/lang/String;
    .end local v32    # "version":Ljava/lang/String;
    :catch_12
    move-exception v8

    .line 456
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    .line 457
    sget-object v33, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v34, "get version error"

    invoke-virtual/range {v33 .. v34}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    goto/16 :goto_9

    .line 446
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v4    # "bt":[B
    .restart local v15    # "file3":Ljava/io/File;
    .restart local v24    # "info":Landroid/content/pm/PackageInfo;
    .restart local v27    # "manager":Landroid/content/pm/PackageManager;
    .restart local v32    # "version":Ljava/lang/String;
    :cond_e
    :try_start_1e
    new-instance v23, Ljava/io/FileOutputStream;

    .line 447
    invoke-virtual {v15}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v33

    const/16 v34, 0x1

    move-object/from16 v0, v23

    move-object/from16 v1, v33

    move/from16 v2, v34

    invoke-direct {v0, v1, v2}, Ljava/io/FileOutputStream;-><init>(Ljava/lang/String;Z)V
    :try_end_1e
    .catch Ljava/lang/Exception; {:try_start_1e .. :try_end_1e} :catch_12

    .line 449
    .restart local v23    # "in":Ljava/io/FileOutputStream;
    const/16 v33, 0x0

    :try_start_1f
    array-length v0, v4

    move/from16 v34, v0

    move-object/from16 v0, v23

    move/from16 v1, v33

    move/from16 v2, v34

    invoke-virtual {v0, v4, v1, v2}, Ljava/io/FileOutputStream;->write([BII)V

    .line 450
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V
    :try_end_1f
    .catch Ljava/io/IOException; {:try_start_1f .. :try_end_1f} :catch_13
    .catch Ljava/lang/Exception; {:try_start_1f .. :try_end_1f} :catch_12

    goto/16 :goto_9

    .line 451
    :catch_13
    move-exception v8

    .line 452
    .local v8, "e":Ljava/io/IOException;
    :try_start_20
    invoke-virtual {v8}, Ljava/io/IOException;->printStackTrace()V
    :try_end_20
    .catch Ljava/lang/Exception; {:try_start_20 .. :try_end_20} :catch_12

    goto/16 :goto_9
.end method
