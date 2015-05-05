/*
 * This contains the code that is to be translated to C
 * It will be removed from the repo once enough of the code has been translated
 */

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.*;

import javax.imageio.ImageIO;
import javax.swing.*;

public class Overworld {
	private static int[] map;
	Random r = new Random();
	int x, y, size, waterlevel;
	public Overworld(int s, int runs, int wl, boolean genStuff, boolean centerBiased) {
		size = s;
		waterlevel = wl;
		map = new int[size*size];
		x = r.nextInt(size-1);
		y = r.nextInt(size-1);
		if(centerBiased==false) {
			for(int z = 0;z<runs;z++) {
				x+=r.nextInt(3)-1;
				y+=r.nextInt(3)-1;
				if(x == size) x = 0;
				if(y == size) y = 0;
				if(x == -1) x = size-1;
				if(y == -1) y = size-1;
				if(map[x+y*size]<255) map[x+y*size]++;
			}
		}
		else {
			for(int z = 0;z<runs;z++) {
				ArrayList<Integer> downhills = new ArrayList<Integer>();
				int xLeft, xRight, yUp, yDown;
				xLeft = x-1;
				xRight = x+1;
				yUp = y-1;
				yDown = y+1;
				if(xLeft==-1) xLeft = size-1;
				if(xRight==size) xRight = 0;
				if(yUp==-1) yUp = size-1;
				if(yDown==size) yDown = 0;
				if(map[xLeft+yUp*size]<=map[x+y*size]) downhills.add(7);
				if(map[x+yUp*size]<=map[x+y*size]) downhills.add(8);
				if(map[xRight+yUp*size]<=map[x+y*size]) downhills.add(9);
				if(map[xLeft+y*size]<=map[x+y*size]) downhills.add(4);
				if(map[xRight+y*size]<=map[x+y*size]) downhills.add(6);
				if(map[xLeft+yDown*size]<=map[x+y*size]) downhills.add(1);
				if(map[x+yDown*size]<=map[x+y*size]) downhills.add(2);
				if(map[xRight+yDown*size]<=map[x+y*size]) downhills.add(3);
				boolean ableToMove = true;
				if(downhills.size()==0) ableToMove = false;
				if(ableToMove==false) map[x+y*size]++;
				else {
					int dir = downhills.get(r.nextInt(downhills.size()));
					if(dir==1) {
						x--;
						y++;
					}
					if(dir==2) y++;
					if(dir==3) {
						x++;
						y++;
					}
					if(dir==4) x--;
					if(dir==6) x++;
					if(dir==7) {
						x--;
						y--;
					}
					if(dir==8) y--;
					if(dir==9) {
						x++;
						y--;
					}
					if(x == size) x = 0;
					if(y == size) y = 0;
					if(x == -1) x = size-1;
					if(y == -1) y = size-1;
					if(map[x+y*size]<255) map[x+y*size]++;
				}
			}
		}
		if(genStuff) {
			cleanUp(4);
			genBeaches();
			genTrees();
			if(centerBiased) normalize();
		}
	}
	
	public int getHeight(int x, int y) {
		return map[x+y*size];
	}
	
	public int getHeight(int i) {
		return map[i];
	}
	
	public void cleanUp(int threshold) {
		for(int x = 0; x < size; x++) {
			for(int y = 0; y < size; y++) {
				int i = x + y * size;
								
				int[][] borders = new int[8][2];
				borders[0][0] = x-1;
				borders[0][1] = y-1;
				
				borders[1][0] = x;
				borders[1][1] = y-1;
				
				borders[2][0] = x+1;
				borders[2][1] = y-1;
				
				borders[3][0] = x-1;
				borders[3][1] = y;
				
				borders[4][0] = x+1;
				borders[4][1] = y;
				
				borders[5][0] = x-1;
				borders[5][1] = y+1;
				
				borders[6][0] = x;
				borders[6][1] = y+1;
				
				borders[7][0] = x+1;
				borders[7][1] = y+1;
				
				for(int a = 0; a < 8;a++) {
					for(int b = 0; b < 2; b++) {
						if(borders[a][b] < 0) borders[a][b] = size;
						if(borders[a][b] >= size) borders[a][b] = 0;
					}
				}
				
				if(map[i] <= waterlevel) {
					int count = 0;
					if(map[borders[0][0] + borders[0][1] * size] > waterlevel) count++;
					if(map[borders[1][0] + borders[1][1] * size] > waterlevel) count++;
					if(map[borders[2][0] + borders[2][1] * size] > waterlevel) count++;
					if(map[borders[3][0] + borders[3][1] * size] > waterlevel) count++;
					if(map[borders[4][0] + borders[4][1] * size] > waterlevel) count++;
					if(map[borders[5][0] + borders[5][1] * size] > waterlevel) count++;
					if(map[borders[6][0] + borders[6][1] * size] > waterlevel) count++;
					if(map[borders[7][0] + borders[7][1] * size] > waterlevel) count++;
					if(count>threshold) map[i] = waterlevel+2;
				}
				if(map[i] > waterlevel) {
					int count = 0;
					if(map[borders[0][0] + borders[0][1] * size] <= waterlevel) count++;
					if(map[borders[1][0] + borders[1][1] * size] <= waterlevel) count++;
					if(map[borders[2][0] + borders[2][1] * size] <= waterlevel) count++;
					if(map[borders[3][0] + borders[3][1] * size] <= waterlevel) count++;
					if(map[borders[4][0] + borders[4][1] * size] <= waterlevel) count++;
					if(map[borders[5][0] + borders[5][1] * size] <= waterlevel) count++;
					if(map[borders[6][0] + borders[6][1] * size] <= waterlevel) count++;
					if(map[borders[7][0] + borders[7][1] * size] <= waterlevel) count++;
					if(count>threshold) map[i] = waterlevel-1;
				}
			}
		}
	}
	
	public void genBeaches() {
		for(int x = 0; x < size; x++) {
			for(int y = 0; y < size; y++) {
				int i = x + y * size;
				
				int[][] borders = new int[8][2];
				borders[0][0] = x-1;
				borders[0][1] = y-1;
				
				borders[1][0] = x;
				borders[1][1] = y-1;
				
				borders[2][0] = x+1;
				borders[2][1] = y-1;
				
				borders[3][0] = x-1;
				borders[3][1] = y;
				
				borders[4][0] = x+1;
				borders[4][1] = y;
				
				borders[5][0] = x-1;
				borders[5][1] = y+1;
				
				borders[6][0] = x;
				borders[6][1] = y+1;
				
				borders[7][0] = x+1;
				borders[7][1] = y+1;
				
				for(int a = 0; a < 8;a++) {
					for(int b = 0; b < 2; b++) {
						if(borders[a][b] < 0) borders[a][b] = size;
						if(borders[a][b] >= size) borders[a][b] = 0;
					}
				}
				
//				if(map[borders[0][0] + borders[0][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300; //Only the four cardinal directions will be beachified
				if(map[borders[1][0] + borders[1][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300;
//				if(map[borders[2][0] + borders[2][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300;
				if(map[borders[3][0] + borders[3][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300;
				if(map[borders[4][0] + borders[4][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300;
//				if(map[borders[5][0] + borders[5][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300;
				if(map[borders[6][0] + borders[6][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300;
//				if(map[borders[7][0] + borders[7][1] * size] <= waterlevel && map[i] > waterlevel) map[i] = 300;
			}
		}
	}
	
	public void genTrees() {
		int numberOfClusters = r.nextInt(size/2)+(size/3);
		for(int n = 0; n < numberOfClusters; n++) {
			int clusterSize = r.nextInt(size/2)+(size/3);
			x = r.nextInt(size-1);
			y = r.nextInt(size-1);
			for(int z = 0; z < clusterSize; z++) {
				x+=r.nextInt(3)-1;
				y+=r.nextInt(3)-1;
				if(x == size) x = 0;
				if(y == size) y = 0;
				if(x == -1) x = size-1;
				if(y == -1) y = size-1;
				if(map[x+y*size]>waterlevel && map[x+y*size] < 300) map[x+y*size]+=301;
			}
		}
	}
	
	public int getColor(int x, int y) {
		int i = x + y * size;
		
//		int color = (255-getHeight(i))*65536+(255-getHeight(i))*256+(255-getHeight(i)); //This is the default green color, changed to red to show any errors on map.
		int color = 0xFF0000;
		if(map[i] <= waterlevel) color = 0*65536+((getHeight(i)*255)/waterlevel)*256+255;
		if(map[i] > waterlevel) color = 0*65536+((getHeight(i)*255)/(256-waterlevel))*256+0;
		if(map[i] == 300) color = 0xFFCC33;
		if(map[i] > 301) color = 0xFF0000+(((getHeight(i)-301)*255)/(256-waterlevel))*256+0;
		
		return color;
	}
	
	public BufferedImage getBigMap() {
		BufferedImage tiles = new BufferedImage(256, 256, BufferedImage.TYPE_INT_RGB);
		try {
			tiles = ImageIO.read(Overworld.class.getResourceAsStream("/tiles.png"));
		} catch (IOException e) {
			e.printStackTrace();
		}
		BufferedImage big = new BufferedImage(size*16, size*16, BufferedImage.TYPE_INT_RGB);
		for(int x = 0;x < size;x++) {
			for(int y = 0; y < size; y++) {
				int i = x + y * size;
				if(map[i] > waterlevel) {
					int color = getColor(x, y);
					int[] currentTile = tiles.getRGB(0, r.nextInt(4)*16, 16, 16, null, 0, 16);
					for(int a = 0; a < 16; a++) {
						for(int b = 0; b < 16; b++) {
							int col = currentTile[a+b*16] & 0xFF;
							currentTile[a+b*16] = ((int)(((double)col/256)*(color >> 8 & 0xFF)))*256;//Apparently canceling out the 256's makes it blue...
						}
					}
					big.setRGB(x*16, y*16, 16, 16, currentTile, 0, 16);
				}
				if(map[i] <= waterlevel) {
					int color = getColor(x, y);
					int[] currentTile = tiles.getRGB(16, r.nextInt(4)*16, 16, 16, null, 0, 16);
					for(int a = 0; a < 16; a++) {
						for(int b = 0; b < 16; b++) {
							int col = currentTile[a+b*16] & 0xFF;
							currentTile[a+b*16] = ((int)(((double)(255-col)/256)*(color >> 8 & 0xFF)));
						}
					}
					big.setRGB(x*16, y*16, 16, 16, currentTile, 0, 16);
				}
				if(map[i] == 300) {
					int[] currentTile = tiles.getRGB(32, r.nextInt(4)*16, 16, 16, null, 0, 16);
					big.setRGB(x*16, y*16, 16, 16, currentTile, 0, 16);
				}
				if(map[i] > 301) {
					int color = getColor(x, y);
					int[] currentTile = tiles.getRGB(48, 0, 16, 16, null, 0, 16);
					for(int a = 0; a < 16; a++) {
						for(int b = 0; b < 16; b++) {
							int col = currentTile[a+b*16] & 0xFF;
							if((currentTile[a+b*16] >> 8 & 0xFF) == (currentTile[a+b*16] & 0xFF)) currentTile[a+b*16] = ((int)(((double)col/256)*(color >> 8 & 0xFF)))*256;
						}
					}
					big.setRGB(x*16, y*16, 16, 16, currentTile, 0, 16);
				}
			}
		}
		return big;
	}
	
	public BufferedImage getPixelMap() {
		BufferedImage img = new BufferedImage(size, size, BufferedImage.TYPE_INT_RGB);
		int[] pixels = new int[size * size];
		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				int i = x + y * size;
				pixels[i] = getColor(x, y);
			}
		}
		img.setRGB(0, 0, size, size, pixels, 0, size);
		return img;
	}
	
	public void normalize() {
		int lowest = 255;
		for(int p = 0; p < size*size; p++) {
			if(map[p] < lowest) lowest = map[p];
		}
		int highest = 0;
		for(int p = 0; p < size*size; p++) {
			if(map[p] > highest) highest = map[p];
		}
		for(int p = 0; p < size*size; p++) {
			map[p] = (int)(((double)(map[p]-lowest))/((double)highest-lowest)*255);
		}
	}
	
	public static void main(String[] args) throws java.io.IOException {
		boolean on = true;
		int s;

		while (on) {
			s = 64;
			Overworld ow = new Overworld(s, 61*s*s, 50, true, false);
			
			//Quad view with a 2x2 grid
//			BufferedImage imgquad = new BufferedImage(s*2, s*2, BufferedImage.TYPE_INT_RGB);
//			imgquad.setRGB(0, 0, s, s, pixels, 0, s);
//			imgquad.setRGB(s, 0, s, s, pixels, 0, s);
//			imgquad.setRGB(0, s, s, s, pixels, 0, s);
//			imgquad.setRGB(s, s, s, s, pixels, 0, s);
			
			//Wrap view with map centered
			/*BufferedImage imgwrap = new BufferedImage(s*2, s*2, BufferedImage.TYPE_INT_RGB);
			imgwrap.setRGB(s/2, s/2, s, s, pixels, 0, s); // center
			imgwrap.setRGB(0, 0, s/2, s/2, pixels, s*s/2+s/2, s); // top left
			imgwrap.setRGB(s/2, 0, s, s/2, pixels, s*s/2, s); // top center
			imgwrap.setRGB(s*3/2, 0, s/2, s/2, pixels, s*s/2, s); // top right
			imgwrap.setRGB(0, s/2, s/2, s, pixels, s/2, s); // left center
			imgwrap.setRGB(s*3/2, s/2, s/2, s, pixels, 0, s); // right center
			imgwrap.setRGB(0, s*3/2, s/2, s/2, pixels, s/2, s); // bottom left
			imgwrap.setRGB(s/2, s*3/2, s, s/2, pixels, 0, s); // bottom center
			imgwrap.setRGB(s*3/2, s*3/2, s/2, s/2, pixels, 0, s); // bottom right
*/		
			
			//first one shows map with pixels, second uses the textures
//			if(JOptionPane.showConfirmDialog(null, null, "Overworld", JOptionPane.CLOSED_OPTION, JOptionPane.PLAIN_MESSAGE, new ImageIcon(ow.getPixelMap().getScaledInstance(512, 512, Image.SCALE_AREA_AVERAGING)))==-1) on = false;
			int o = JOptionPane.showConfirmDialog(null, "Save to file?", "Overworld", JOptionPane.YES_OPTION, JOptionPane.PLAIN_MESSAGE, new ImageIcon(ow.getBigMap().getScaledInstance(512, 512, Image.SCALE_AREA_AVERAGING)));
			if(o==-1) on = false;
			if(o==0) ImageIO.write(ow.getBigMap(), "png", new File("map.png"));
			
		}
	}
}
