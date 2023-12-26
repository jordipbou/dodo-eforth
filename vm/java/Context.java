import java.nio.ByteBuffer;

public class Context {
	public int[] s;
	public int ss;
	public int sp;
	public int[] r;
	public int rs;
	public int rp;
	public ByteBuffer u;
	public int us;
	public int ip;
	/* TODO: Dictionary */

	public Context(int ssize, int rsize, int usize) {
		s = new int[ssize];
		ss = ssize;
		sp = 0;
		r = new int[rsize];
		rs = rsize;
		rp = 0;
		u = ByteBuffer.allocateDirect(usize*4);
		us = usize;
		ip = -us*4 - 1;
	}

	public void push(int v) { s[sp++] = v; }
	public int pop() { return s[--sp]; }

	public int dict_size() { return 0; }
	public boolean valid_ip() { return ip >= -us*4 && ip < dict_size(); }

	public byte bfetch(int addr) {
		if (addr < 0) {
			return u.get(us*4 + addr);
		} else {
			/* Return from the dictionary */
			return 0;
		}
	}

	public void bstore(int addr, byte v) {
		if (addr < 0) {
			u.put(us*4 + addr, v);
		} else {
		}
	}

	public int cfetch(int addr) {
		if (addr < 0) {
			return u.getInt(us*4 + addr);
		} else {
			return 0;
		}
	}

	public void cstore(int addr, int v) {
		if (addr < 0) {
			u.putInt(us*4 + addr, v);
		} else {
		}
	}

	public byte peek() { return bfetch(ip); }
	public byte token() { byte t = bfetch(ip); ip++; return t; }

	public boolean tail() { return !valid_ip() || peek() == ']'; }
	public void call() { int q = pop(); if (!tail()) r[rp++] = ip; ip = q; }
	public void eval(int q) { push(q); call(); inner(); }
	public void ret() { if (rp > 0) ip = r[--rp]; else ip = -us*4 - 1; }

	public void block() {
		int t = 1;
		push(ip);
		while(t > 0) {
			switch (token()) {
				case '{': t++; break;
				case '}': t--; break;
			}
		}
	}

	public void choice(int b, int f, int t) { if (b == 0) eval(f); else eval(t); }
	public void times(int n, int q) { for(;n > 0; n--) eval(q); }

	public void step() {
		switch (token()) {
			/* System interface */
			case 'q': System.exit(0); break;
			case 'c': push(4); break;
			case 'u': { int n = pop(); push(-us*4 + n*4); } break;

			/* Literals */
			case 't': push(-1); break;
			case 'f': push(0); break;
			case '1': push(1); break;

			/* Memory access */
			case ',': { int a = pop(); int b = pop(); cstore(a, b); } break;
			case '.': { int a = pop(); push(cfetch(a)); } break;
			case ';': { int a = pop(); int b = pop(); bstore(a, (byte)b); } break;
			case ':': { int a = pop(); push(bfetch(a)); } break;

			/* Return stack */
			case '(': r[rp++] = s[--sp]; break;
			case '@': s[sp++] = r[rp - 1]; break;
			case ')': s[sp++] = r[--rp]; break;

			/* Data stack */
			case '_': pop(); break;
			case 'd': { int a = pop(); push(a); push(a); } break;
			case 'o': { int a = pop(); int b = pop(); push(b); push(a); push(b); } break;
			case 's': { int a = pop(); int b = pop(); push(a); push(b); } break;

			/* Logic */
			case '0': { int a = pop(); push(a == 0 ? -1 : 0); } break;
			case '<': { int a = pop(); int b = pop(); push(b < a ? -1 : 0); } break;
			case '=': { int a = pop(); int b = pop(); push(b == a ? -1 : 0); } break;
			case '>': { int a = pop(); int b = pop(); push(b > a ? -1 : 0); } break;
			case '&': { int a = pop(); int b = pop(); push(b & a); } break;
			case '|': { int a = pop(); int b = pop(); push(b | a); } break;
			case '^': { int a = pop(); int b = pop(); push(b ^ a); } break;
			case '~': { int a = pop(); push(~a); } break;

			/* Arithmetic */
			case '+': { int a = pop(); int b = pop(); push(b + a); } break;
			case '-': { int a = pop(); int b = pop(); push(b - a); } break;
			case '*': { int a = pop(); int b = pop(); push(b * a); } break;
			case '/': { int a = pop(); int b = pop(); push(b / a); } break;
			case '%': { int a = pop(); int b = pop(); push(b % a); } break;

			/* Inner interpreters */
			case 'z': { int a = pop(); int b = pop(); if (b == 0) ip += a; } break;
			case 'j': { int a = pop(); ip += a; } break;
			case 'i': call(); break;
			case '{': block(); break;
			case '}': case ']': ret(); break;

			/* Combinators */
			case '?': { int f = pop(); int t = pop(); int b = pop(); choice(b, t, f); } break;
			case 'n': { int q = pop(); int n = pop(); times(n, q); } break;
		}
	}

	public void dump_code(int p) {
		int t = 1;
		byte k;
		while (t > 0) {
			switch (k = bfetch(p)) {
				case ']': t--; System.out.printf("]"); break;
				case '[': t++; System.out.printf("["); break;
				default: System.out.printf("%c", k); break;
			}
			p++;
		}
	}

	public void trace() {
		for (int i = 0; i < sp; i++) { System.out.printf("%d ", s[i]); }
		if (valid_ip()) { System.out.printf(": "); dump_code(ip); }
		for (int i = rp - 1; i >= 0; i--) { System.out.printf(" : "); dump_code(r[i]); }
	}

	public void inner() {
		int t = rp;
		while (t <= rp && valid_ip()) {
			trace(); System.out.println();
			step();
		}
	}

	public void assembler(String s) {
		int l = s.length();
		for (int i = 0; i < l; i++) { u.put(us*4 - l - 1 + i, (byte)s.charAt(i)); }
		u.put(us*4 - 1, (byte)']');
		eval(-l - 1);
	}
}
