#pragma once
#include<vector>
#include"freqTabel.h"

namespace Music1
{
	static constexpr float N2 = 1.0f;		//二分
	static constexpr float N4 = 0.5f;		//四分
	static constexpr float N8 = 0.25f;		//八分
	static constexpr float N16 = 0.125f;	//十六分
	static constexpr float N8D = 0.375f;	//付点八分

	static const std::vector<note> NoteTable =
	{
		//1小節
		make_note("C4",N4),
		make_note("Eb4",N4),
		make_note("F4",N8D),
		make_note("Eb4",N16),
		make_note("Eb4",N8),
		make_note("F4",N8),

		//2
		make_note("F4",N8),
		make_note("F4",N8),
		make_note("Bb4",N8),
		make_note("Ab4",N8),
		make_note("G4",N16),
		make_note("F4",N8),
		make_note("G4", N16 +N4),

		//3
		make_note("G4",N4),
		make_note("Bb4",N4),
		make_note("C5",N8D),
		make_note("F4",N8D),
		make_note("Eb4",N8),

		//4
		make_note("D4",N8),
		make_note("D4",N8),
		make_note("C4",N8),
		make_note("D4",N8),
		make_note("F4",N16),
		make_note("Eb4",N8),
		make_note("Eb4",N16 + N4),

		//5
		make_note("C4",N4),
		make_note("Eb4",N4),
		make_note("F4",N8D),
		make_note("Eb4",N16),
		make_note("Eb4",N8),
		make_note("F4",N8),

		//6
		make_note("F4",N8),
		make_note("F4",N8),
		make_note("Bb4",N8),
		make_note("Ab4",N8),
		make_note("G4",N16),
		make_note("F4",N8),
		make_note("G4", N16 + N4),

		//7
		make_note("G4",N4),
		make_note("Bb4",N4),
		make_note("C5",N8D),
		make_note("F4",N8D),
		make_note("Eb4",N8),

		//8
		make_note("Bb4",N8),
		make_note("Bb4",N8),
		make_note("G4",N8),
		make_note("Bb4",N8),
		make_note("Bb4",N8D),
		make_note("C5",N16+N4),

		//9
		make_note("C4",N4),
		make_note("Eb4",N4),
		make_note("F4",N8D),
		make_note("Eb4",N16),
		make_note("Eb4",N8),
		make_note("F4",N8),

		//10
		make_note("F4",N8),
		make_note("F4",N8),
		make_note("Bb4",N8),
		make_note("Ab4",N8),
		make_note("G4",N16),
		make_note("F4",N8),
		make_note("G4", N16 + N4),

		//11
		make_note("G4",N4),
		make_note("Bb4",N4),
		make_note("C5",N8D),
		make_note("F4",N8D),
		make_note("Eb4",N8),

		//12
		make_note("Bb4",N8),
		make_note("Bb4",N8),
		make_note("G4",N8),
		make_note("Bb4",N8),
		make_note("Bb4",N8D),
		make_note("C5",N16 + N4),
	};
	static const std::vector<note> base1 =
	{
		//1
		make_note("C3",N2),
		make_note("F3",N2),

		//2
		make_note("Bb3",N2),
		make_note("Eb3",N2),

		//3
		make_note("C3",N2),
		make_note("F3",N2),

		//4
		make_note("Bb3",N2),
		make_note("Eb3",N2),

		//5
		make_note("C3",N2),
		make_note("F3",N2),

		//6
		make_note("Bb3",N2),
		make_note("Eb3",N2),

		//7
		make_note("C3",N2),
		make_note("F3",N2),

		//8
		make_note("Bb3",N2),
		make_note("C3",N2),

		//9
		make_note("C3",N2),
		make_note("F3",N2),

		//10
		make_note("Bb3",N2),
		make_note("Eb3",N2),

		//11
		make_note("C3",N2),
		make_note("F3",N2),

		//12
		make_note("Bb3",N2),
		make_note("C3",N2),
	};
	static const std::vector<note> base2 =
	{
		//1
		make_note("Eb3",N2),
		make_note("Ab3",N2),

		//2
		make_note("D3",N2),
		make_note("G3",N2),

		//3
		make_note("Eb3",N2),
		make_note("Ab3",N2),

		//4
		make_note("D3",N2),
		make_note("G3",N2),

		//5
		make_note("Eb3",N2),
		make_note("Ab3",N2),

		//6
		make_note("D3",N2),
		make_note("G3",N2),

		//7
		make_note("Eb3",N2),
		make_note("Ab3",N2),

		//8
		make_note("D3",N2),
		make_note("Eb3",N2),

		//9
		make_note("Eb3",N2),
		make_note("Ab3",N2),

		//10
		make_note("D3",N2),
		make_note("G3",N2),

		//11
		make_note("Eb3",N2),
		make_note("Ab3",N2),

		//12
		make_note("D3",N2),
		make_note("Eb3",N2)
	};
}