.class public Lcom/example/forshell/Refinvoke;
.super Ljava/lang/Object;
.source "Refinvoke.java"


# direct methods
.method public constructor <init>()V
    .locals 0

    .prologue
    .line 7
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method

.method public static getFieldOjbect(Ljava/lang/String;Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;
    .locals 4
    .param p0, "class_name"    # Ljava/lang/String;
    .param p1, "obj"    # Ljava/lang/Object;
    .param p2, "filedName"    # Ljava/lang/String;

    .prologue
    .line 72
    :try_start_0
    invoke-static {p0}, Ljava/lang/Class;->forName(Ljava/lang/String;)Ljava/lang/Class;

    move-result-object v2

    .line 73
    .local v2, "obj_class":Ljava/lang/Class;
    invoke-virtual {v2, p2}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v1

    .line 74
    .local v1, "field":Ljava/lang/reflect/Field;
    const/4 v3, 0x1

    invoke-virtual {v1, v3}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 75
    invoke-virtual {v1, p1}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;
    :try_end_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/NoSuchFieldException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_3
    .catch Ljava/lang/ClassNotFoundException; {:try_start_0 .. :try_end_0} :catch_4

    move-result-object v3

    .line 92
    .end local v1    # "field":Ljava/lang/reflect/Field;
    .end local v2    # "obj_class":Ljava/lang/Class;
    :goto_0
    return-object v3

    .line 76
    :catch_0
    move-exception v0

    .line 78
    .local v0, "e":Ljava/lang/SecurityException;
    invoke-virtual {v0}, Ljava/lang/SecurityException;->printStackTrace()V

    .line 92
    .end local v0    # "e":Ljava/lang/SecurityException;
    :goto_1
    const/4 v3, 0x0

    goto :goto_0

    .line 79
    :catch_1
    move-exception v0

    .line 81
    .local v0, "e":Ljava/lang/NoSuchFieldException;
    invoke-virtual {v0}, Ljava/lang/NoSuchFieldException;->printStackTrace()V

    goto :goto_1

    .line 82
    .end local v0    # "e":Ljava/lang/NoSuchFieldException;
    :catch_2
    move-exception v0

    .line 84
    .local v0, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v0}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_1

    .line 85
    .end local v0    # "e":Ljava/lang/IllegalArgumentException;
    :catch_3
    move-exception v0

    .line 87
    .local v0, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v0}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_1

    .line 88
    .end local v0    # "e":Ljava/lang/IllegalAccessException;
    :catch_4
    move-exception v0

    .line 90
    .local v0, "e":Ljava/lang/ClassNotFoundException;
    invoke-virtual {v0}, Ljava/lang/ClassNotFoundException;->printStackTrace()V

    goto :goto_1
.end method

.method public static getStaticFieldOjbect(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;
    .locals 5
    .param p0, "class_name"    # Ljava/lang/String;
    .param p1, "filedName"    # Ljava/lang/String;

    .prologue
    const/4 v3, 0x0

    .line 100
    :try_start_0
    invoke-static {p0}, Ljava/lang/Class;->forName(Ljava/lang/String;)Ljava/lang/Class;

    move-result-object v2

    .line 101
    .local v2, "obj_class":Ljava/lang/Class;
    invoke-virtual {v2, p1}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v1

    .line 102
    .local v1, "field":Ljava/lang/reflect/Field;
    const/4 v4, 0x1

    invoke-virtual {v1, v4}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 103
    const/4 v4, 0x0

    invoke-virtual {v1, v4}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;
    :try_end_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/NoSuchFieldException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_3
    .catch Ljava/lang/ClassNotFoundException; {:try_start_0 .. :try_end_0} :catch_4

    move-result-object v3

    .line 120
    .end local v1    # "field":Ljava/lang/reflect/Field;
    .end local v2    # "obj_class":Ljava/lang/Class;
    :goto_0
    return-object v3

    .line 104
    :catch_0
    move-exception v0

    .line 106
    .local v0, "e":Ljava/lang/SecurityException;
    invoke-virtual {v0}, Ljava/lang/SecurityException;->printStackTrace()V

    goto :goto_0

    .line 107
    .end local v0    # "e":Ljava/lang/SecurityException;
    :catch_1
    move-exception v0

    .line 109
    .local v0, "e":Ljava/lang/NoSuchFieldException;
    invoke-virtual {v0}, Ljava/lang/NoSuchFieldException;->printStackTrace()V

    goto :goto_0

    .line 110
    .end local v0    # "e":Ljava/lang/NoSuchFieldException;
    :catch_2
    move-exception v0

    .line 112
    .local v0, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v0}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_0

    .line 113
    .end local v0    # "e":Ljava/lang/IllegalArgumentException;
    :catch_3
    move-exception v0

    .line 115
    .local v0, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v0}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_0

    .line 116
    .end local v0    # "e":Ljava/lang/IllegalAccessException;
    :catch_4
    move-exception v0

    .line 118
    .local v0, "e":Ljava/lang/ClassNotFoundException;
    invoke-virtual {v0}, Ljava/lang/ClassNotFoundException;->printStackTrace()V

    goto :goto_0
.end method

.method public static invokeMethod(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Object;[Ljava/lang/Class;[Ljava/lang/Object;)Ljava/lang/Object;
    .locals 4
    .param p0, "class_name"    # Ljava/lang/String;
    .param p1, "method_name"    # Ljava/lang/String;
    .param p2, "obj"    # Ljava/lang/Object;
    .param p3, "pareTyple"    # [Ljava/lang/Class;
    .param p4, "pareVaules"    # [Ljava/lang/Object;

    .prologue
    .line 43
    :try_start_0
    invoke-static {p0}, Ljava/lang/Class;->forName(Ljava/lang/String;)Ljava/lang/Class;

    move-result-object v2

    .line 44
    .local v2, "obj_class":Ljava/lang/Class;
    invoke-virtual {v2, p1, p3}, Ljava/lang/Class;->getMethod(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;

    move-result-object v1

    .line 45
    .local v1, "method":Ljava/lang/reflect/Method;
    invoke-virtual {v1, p2, p4}, Ljava/lang/reflect/Method;->invoke(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;
    :try_end_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/NoSuchMethodException; {:try_start_0 .. :try_end_0} :catch_3
    .catch Ljava/lang/reflect/InvocationTargetException; {:try_start_0 .. :try_end_0} :catch_4
    .catch Ljava/lang/ClassNotFoundException; {:try_start_0 .. :try_end_0} :catch_5

    move-result-object v3

    .line 65
    .end local v1    # "method":Ljava/lang/reflect/Method;
    .end local v2    # "obj_class":Ljava/lang/Class;
    :goto_0
    return-object v3

    .line 46
    :catch_0
    move-exception v0

    .line 48
    .local v0, "e":Ljava/lang/SecurityException;
    invoke-virtual {v0}, Ljava/lang/SecurityException;->printStackTrace()V

    .line 65
    .end local v0    # "e":Ljava/lang/SecurityException;
    :goto_1
    const/4 v3, 0x0

    goto :goto_0

    .line 49
    :catch_1
    move-exception v0

    .line 51
    .local v0, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v0}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_1

    .line 52
    .end local v0    # "e":Ljava/lang/IllegalArgumentException;
    :catch_2
    move-exception v0

    .line 54
    .local v0, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v0}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_1

    .line 55
    .end local v0    # "e":Ljava/lang/IllegalAccessException;
    :catch_3
    move-exception v0

    .line 57
    .local v0, "e":Ljava/lang/NoSuchMethodException;
    invoke-virtual {v0}, Ljava/lang/NoSuchMethodException;->printStackTrace()V

    goto :goto_1

    .line 58
    .end local v0    # "e":Ljava/lang/NoSuchMethodException;
    :catch_4
    move-exception v0

    .line 60
    .local v0, "e":Ljava/lang/reflect/InvocationTargetException;
    invoke-virtual {v0}, Ljava/lang/reflect/InvocationTargetException;->printStackTrace()V

    goto :goto_1

    .line 61
    .end local v0    # "e":Ljava/lang/reflect/InvocationTargetException;
    :catch_5
    move-exception v0

    .line 63
    .local v0, "e":Ljava/lang/ClassNotFoundException;
    invoke-virtual {v0}, Ljava/lang/ClassNotFoundException;->printStackTrace()V

    goto :goto_1
.end method

.method public static invokeStaticMethod(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/Class;[Ljava/lang/Object;)Ljava/lang/Object;
    .locals 5
    .param p0, "class_name"    # Ljava/lang/String;
    .param p1, "method_name"    # Ljava/lang/String;
    .param p2, "pareTyple"    # [Ljava/lang/Class;
    .param p3, "pareVaules"    # [Ljava/lang/Object;

    .prologue
    const/4 v3, 0x0

    .line 13
    :try_start_0
    invoke-static {p0}, Ljava/lang/Class;->forName(Ljava/lang/String;)Ljava/lang/Class;

    move-result-object v2

    .line 14
    .local v2, "obj_class":Ljava/lang/Class;
    invoke-virtual {v2, p1, p2}, Ljava/lang/Class;->getMethod(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;

    move-result-object v1

    .line 15
    .local v1, "method":Ljava/lang/reflect/Method;
    const/4 v4, 0x0

    invoke-virtual {v1, v4, p3}, Ljava/lang/reflect/Method;->invoke(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;
    :try_end_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/NoSuchMethodException; {:try_start_0 .. :try_end_0} :catch_3
    .catch Ljava/lang/reflect/InvocationTargetException; {:try_start_0 .. :try_end_0} :catch_4
    .catch Ljava/lang/ClassNotFoundException; {:try_start_0 .. :try_end_0} :catch_5

    move-result-object v3

    .line 35
    .end local v1    # "method":Ljava/lang/reflect/Method;
    .end local v2    # "obj_class":Ljava/lang/Class;
    :goto_0
    return-object v3

    .line 16
    :catch_0
    move-exception v0

    .line 18
    .local v0, "e":Ljava/lang/SecurityException;
    invoke-virtual {v0}, Ljava/lang/SecurityException;->printStackTrace()V

    goto :goto_0

    .line 19
    .end local v0    # "e":Ljava/lang/SecurityException;
    :catch_1
    move-exception v0

    .line 21
    .local v0, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v0}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_0

    .line 22
    .end local v0    # "e":Ljava/lang/IllegalArgumentException;
    :catch_2
    move-exception v0

    .line 24
    .local v0, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v0}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_0

    .line 25
    .end local v0    # "e":Ljava/lang/IllegalAccessException;
    :catch_3
    move-exception v0

    .line 27
    .local v0, "e":Ljava/lang/NoSuchMethodException;
    invoke-virtual {v0}, Ljava/lang/NoSuchMethodException;->printStackTrace()V

    goto :goto_0

    .line 28
    .end local v0    # "e":Ljava/lang/NoSuchMethodException;
    :catch_4
    move-exception v0

    .line 30
    .local v0, "e":Ljava/lang/reflect/InvocationTargetException;
    invoke-virtual {v0}, Ljava/lang/reflect/InvocationTargetException;->printStackTrace()V

    goto :goto_0

    .line 31
    .end local v0    # "e":Ljava/lang/reflect/InvocationTargetException;
    :catch_5
    move-exception v0

    .line 33
    .local v0, "e":Ljava/lang/ClassNotFoundException;
    invoke-virtual {v0}, Ljava/lang/ClassNotFoundException;->printStackTrace()V

    goto :goto_0
.end method

.method public static setFieldOjbect(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Object;Ljava/lang/Object;)V
    .locals 4
    .param p0, "classname"    # Ljava/lang/String;
    .param p1, "filedName"    # Ljava/lang/String;
    .param p2, "obj"    # Ljava/lang/Object;
    .param p3, "filedVaule"    # Ljava/lang/Object;

    .prologue
    .line 127
    :try_start_0
    invoke-static {p0}, Ljava/lang/Class;->forName(Ljava/lang/String;)Ljava/lang/Class;

    move-result-object v2

    .line 128
    .local v2, "obj_class":Ljava/lang/Class;
    invoke-virtual {v2, p1}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v1

    .line 129
    .local v1, "field":Ljava/lang/reflect/Field;
    const/4 v3, 0x1

    invoke-virtual {v1, v3}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 130
    invoke-virtual {v1, p2, p3}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V
    :try_end_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/NoSuchFieldException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_3
    .catch Ljava/lang/ClassNotFoundException; {:try_start_0 .. :try_end_0} :catch_4

    .line 147
    .end local v1    # "field":Ljava/lang/reflect/Field;
    .end local v2    # "obj_class":Ljava/lang/Class;
    :goto_0
    return-void

    .line 131
    :catch_0
    move-exception v0

    .line 133
    .local v0, "e":Ljava/lang/SecurityException;
    invoke-virtual {v0}, Ljava/lang/SecurityException;->printStackTrace()V

    goto :goto_0

    .line 134
    .end local v0    # "e":Ljava/lang/SecurityException;
    :catch_1
    move-exception v0

    .line 136
    .local v0, "e":Ljava/lang/NoSuchFieldException;
    invoke-virtual {v0}, Ljava/lang/NoSuchFieldException;->printStackTrace()V

    goto :goto_0

    .line 137
    .end local v0    # "e":Ljava/lang/NoSuchFieldException;
    :catch_2
    move-exception v0

    .line 139
    .local v0, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v0}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_0

    .line 140
    .end local v0    # "e":Ljava/lang/IllegalArgumentException;
    :catch_3
    move-exception v0

    .line 142
    .local v0, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v0}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_0

    .line 143
    .end local v0    # "e":Ljava/lang/IllegalAccessException;
    :catch_4
    move-exception v0

    .line 145
    .local v0, "e":Ljava/lang/ClassNotFoundException;
    invoke-virtual {v0}, Ljava/lang/ClassNotFoundException;->printStackTrace()V

    goto :goto_0
.end method

.method public static setStaticOjbect(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Object;)V
    .locals 4
    .param p0, "class_name"    # Ljava/lang/String;
    .param p1, "filedName"    # Ljava/lang/String;
    .param p2, "filedVaule"    # Ljava/lang/Object;

    .prologue
    .line 152
    :try_start_0
    invoke-static {p0}, Ljava/lang/Class;->forName(Ljava/lang/String;)Ljava/lang/Class;

    move-result-object v2

    .line 153
    .local v2, "obj_class":Ljava/lang/Class;
    invoke-virtual {v2, p1}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v1

    .line 154
    .local v1, "field":Ljava/lang/reflect/Field;
    const/4 v3, 0x1

    invoke-virtual {v1, v3}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 155
    const/4 v3, 0x0

    invoke-virtual {v1, v3, p2}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V
    :try_end_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/NoSuchFieldException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_3
    .catch Ljava/lang/ClassNotFoundException; {:try_start_0 .. :try_end_0} :catch_4

    .line 172
    .end local v1    # "field":Ljava/lang/reflect/Field;
    .end local v2    # "obj_class":Ljava/lang/Class;
    :goto_0
    return-void

    .line 156
    :catch_0
    move-exception v0

    .line 158
    .local v0, "e":Ljava/lang/SecurityException;
    invoke-virtual {v0}, Ljava/lang/SecurityException;->printStackTrace()V

    goto :goto_0

    .line 159
    .end local v0    # "e":Ljava/lang/SecurityException;
    :catch_1
    move-exception v0

    .line 161
    .local v0, "e":Ljava/lang/NoSuchFieldException;
    invoke-virtual {v0}, Ljava/lang/NoSuchFieldException;->printStackTrace()V

    goto :goto_0

    .line 162
    .end local v0    # "e":Ljava/lang/NoSuchFieldException;
    :catch_2
    move-exception v0

    .line 164
    .local v0, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v0}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_0

    .line 165
    .end local v0    # "e":Ljava/lang/IllegalArgumentException;
    :catch_3
    move-exception v0

    .line 167
    .local v0, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v0}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_0

    .line 168
    .end local v0    # "e":Ljava/lang/IllegalAccessException;
    :catch_4
    move-exception v0

    .line 170
    .local v0, "e":Ljava/lang/ClassNotFoundException;
    invoke-virtual {v0}, Ljava/lang/ClassNotFoundException;->printStackTrace()V

    goto :goto_0
.end method
