package com.example.forshell;

import java.lang.reflect.Field;

public class Smith<T> {
	private Object obj;
	private String fieldName;

	private boolean inited;
	private Field field;

	public Smith(Object obj, String fieldName) {
		if (obj == null) {
			throw new IllegalArgumentException("obj cannot be null");
		}
		this.obj = obj;
		this.fieldName = fieldName;
	}

	private void prepare() {
		if (inited)
			return;
		inited = true;

		Class<?> c = obj.getClass();
		while (c != null) {
			try {
				Field f = c.getDeclaredField(fieldName);// Returns a {@code
														// Field} object for the
														// field with the
														// specified name
				f.setAccessible(true);
				field = f;
				return;
			} catch (Exception e) {
			} finally {
				c = c.getSuperclass();
			}
		}
	}

	public T get() throws NoSuchFieldException, IllegalAccessException,
			IllegalArgumentException {
		prepare();

		if (field == null)
			throw new NoSuchFieldException();

		try {
			@SuppressWarnings("unchecked")
			T r = (T) field.get(obj);// Returns the value of the field in the
										// specified object.
			return r;
		} catch (ClassCastException e) {
			throw new IllegalArgumentException("unable to cast object");
		}
	}

	public void set(T val) throws NoSuchFieldException, IllegalAccessException,//
			IllegalArgumentException {
		prepare();

		if (field == null)
			throw new NoSuchFieldException();

		field.set(obj, val);// Sets the value of the field in the specified
							// object to the value.
	}

	public static Field getField(Class<?> cls, String name) {

		for (Field field : cls.getDeclaredFields()) {
			if (!field.isAccessible()) {
				field.setAccessible(true);
			}
			if (field.getName().equals(name)) {
				return field;
			}
		}
		return null;
	}
}